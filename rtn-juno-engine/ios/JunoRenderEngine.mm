#import "JunoRenderEngine.hpp"
#import <Foundation/Foundation.h>
#import <cstring>
#include <algorithm>

JunoRenderEngine::JunoRenderEngine()
: device_(nil),
  queue_(nil),
  lib_(nil),
  pipeline_(nil),
  readyIndex_(-1),
  polyphony_(8),
  maxFrames_(512),
  sampleRate_(44100.0f),
  masterVolume_(0.8f),
  subLevel_(0.0f),
  init_(false),
  inflight_(nil),
  submitIndex_(0) {}

bool JunoRenderEngine::initialize(float sr, int poly, int maxFramesPerBuffer) {
    device_ = MTLCreateSystemDefaultDevice();
    if (!device_) {
        NSLog(@"[JunoRenderEngine] No Metal device available");
        return false;
    }

    NSError *err = nil;
    queue_ = [device_ newCommandQueue];
    lib_   = [device_ newDefaultLibrary];
    if (!lib_) {
        NSLog(@"[JunoRenderEngine] Failed to create default Metal library");
        return false;
    }

    id<MTLFunction> fn = [lib_ newFunctionWithName:@"renderOscillatorBank"];
    if (!fn) {
        NSLog(@"[JunoRenderEngine] Missing Metal function 'renderOscillatorBank'");
        return false;
    }

    pipeline_ = [device_ newComputePipelineStateWithFunction:fn error:&err];
    if (err || !pipeline_) {
        NSLog(@"[JunoRenderEngine] Metal pipeline error: %@", err);
        return false;
    }

    polyphony_ = poly;
    sampleRate_ = sr;
    maxFrames_ = std::max(1, maxFramesPerBuffer);

    NSUInteger voiceBufLen  = sizeof(VoiceGPUParams) * polyphony_;
    NSUInteger globalBufLen = sizeof(GlobalGPUParams);
    NSUInteger outBufLen    = sizeof(float) * 2 * maxFrames_;

    for (int i = 0; i < 3; ++i) {
        voiceBuf_[i] = [device_ newBufferWithLength:voiceBufLen
                                            options:MTLResourceStorageModeShared];
        globalBuf_[i] = [device_ newBufferWithLength:globalBufLen
                                             options:MTLResourceStorageModeShared];
        outBuf_[i] = [device_ newBufferWithLength:outBufLen
                                          options:MTLResourceStorageModeShared];
    }

    inflight_ = dispatch_semaphore_create(3);
    readyIndex_.store(-1, std::memory_order_release);
    submitIndex_ = 0;
    voiceCache_.assign(polyphony_, VoiceGPUParams{});

    init_ = true;
    return true;
}

void JunoRenderEngine::updateVoices(const std::vector<VoiceGPUParams> &v) {
    if (!init_) return;
    std::lock_guard<std::mutex> g(lock_);
    voiceCache_.assign(polyphony_, VoiceGPUParams{});
    std::size_t count = std::min<std::size_t>(v.size(), static_cast<std::size_t>(polyphony_));
    if (count > 0) {
        std::copy_n(v.begin(), count, voiceCache_.begin());
    }
}

void JunoRenderEngine::render(float *L, float *R, int n) {
    if (!init_ || !L || !R || n <= 0) return;

    // Copy from the most recently completed GPU buffer if available.
    int readyIdx = readyIndex_.exchange(-1, std::memory_order_acq_rel);
    if (readyIdx >= 0 && outBuf_[readyIdx]) {
        float *out = (float *)[outBuf_[readyIdx] contents];
        int frames = std::min(n, maxFrames_);
        for (int f = 0; f < frames; ++f) {
            L[f] = out[f * 2 + 0];
            R[f] = out[f * 2 + 1];
        }
        if (frames < n) {
            std::fill(L + frames, L + n, 0.0f);
            std::fill(R + frames, R + n, 0.0f);
        }
    } else {
        std::fill(L, L + n, 0.0f);
        std::fill(R, R + n, 0.0f);
    }

    // If all buffers are in flight, skip scheduling to avoid blocking the audio thread.
    if (!inflight_ || dispatch_semaphore_wait(inflight_, DISPATCH_TIME_NOW) != 0) {
        return;
    }

    int bufIndex = submitIndex_;
    submitIndex_ = (submitIndex_ + 1) % 3;

    GlobalGPUParams globals{};
    globals.sampleRate   = sampleRate_;
    globals.lfoValue     = 0.0f; // could be driven externally
    globals.masterVolume = masterVolume_;
    globals.subLevel     = subLevel_;
    globals.frameCount   = static_cast<uint32_t>(std::min(n, maxFrames_));
    globals.voiceCount   = static_cast<uint32_t>(polyphony_);

    if (globalBuf_[bufIndex]) {
        std::memcpy([globalBuf_[bufIndex] contents],
                    &globals,
                    sizeof(GlobalGPUParams));
    }

    // Copy the latest voice state into the GPU buffer for this submission.
    {
        std::lock_guard<std::mutex> g(lock_);
        std::size_t bytes = sizeof(VoiceGPUParams) * std::min<std::size_t>(voiceCache_.size(), static_cast<std::size_t>(polyphony_));
        if (voiceBuf_[bufIndex] && bytes > 0) {
            std::memcpy([voiceBuf_[bufIndex] contents], voiceCache_.data(), bytes);
        }
    }

    id<MTLCommandBuffer> cmd = [queue_ commandBuffer];
    id<MTLComputeCommandEncoder> enc = [cmd computeCommandEncoder];
    [enc setComputePipelineState:pipeline_];
    [enc setBuffer:voiceBuf_[bufIndex]  offset:0 atIndex:0];
    [enc setBuffer:globalBuf_[bufIndex] offset:0 atIndex:1];
    [enc setBuffer:outBuf_[bufIndex]    offset:0 atIndex:2];

    // One thread per voice; kernel is expected to handle per-frame loop.
    MTLSize grid = MTLSizeMake(polyphony_, 1, 1);
    MTLSize tpg  = MTLSizeMake(1, 1, 1);
    [enc dispatchThreads:grid threadsPerThreadgroup:tpg];
    [enc endEncoding];

    int completionIdx = bufIndex;
    __weak typeof(self) weakSelf = self;
    [cmd addCompletedHandler:^(id<MTLCommandBuffer> _Nonnull) {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (!strongSelf) return;
        strongSelf->readyIndex_.store(completionIdx, std::memory_order_release);
        dispatch_semaphore_signal(strongSelf->inflight_);
    }];

    [cmd commit];
}

void JunoRenderEngine::shutdown() {
    if (queue_) {
        [queue_ waitUntilIdle];
    }

    readyIndex_.store(-1, std::memory_order_release);
    submitIndex_ = 0;
    voiceCache_.clear();

    init_ = false;
    device_ = nil;
    queue_ = nil;
    lib_ = nil;
    pipeline_ = nil;
    for (int i = 0; i < 3; ++i) {
        voiceBuf_[i] = nil;
        globalBuf_[i] = nil;
        outBuf_[i] = nil;
    }
    inflight_ = nil;
}
