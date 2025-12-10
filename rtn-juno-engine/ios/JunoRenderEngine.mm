#import "JunoRenderEngine.hpp"
#import <Foundation/Foundation.h>
#import <cstring>

JunoRenderEngine::JunoRenderEngine()
    : device_(nil),
      queue_(nil),
      lib_(nil),
      pipeline_(nil),
      current_(0),
      polyphony_(8),
      maxFrames_(512),
      sampleRate_(44100.0f),
      masterVolume_(0.8f),
      subLevel_(0.0f),
      init_(false) {}

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

    init_ = true;
    return true;
}

void JunoRenderEngine::updateVoices(const std::vector<VoiceGPUParams> &v) {
    if (!init_) return;
    std::lock_guard<std::mutex> g(lock_);
    int nextIdx = (current_.load(std::memory_order_relaxed) + 1) % 3;
    std::size_t bytes = sizeof(VoiceGPUParams) * std::min<int>(polyphony_, (int)v.size());
    if (voiceBuf_[nextIdx] && bytes > 0) {
        std::memcpy([voiceBuf_[nextIdx] contents], v.data(), bytes);
    }
}

void JunoRenderEngine::render(float *L, float *R, int n) {
    if (!init_ || !L || !R || n <= 0) return;

    int bufIndex = current_.load(std::memory_order_acquire);

    GlobalGPUParams globals{};
    globals.sampleRate  = sampleRate_;
    globals.lfoValue    = 0.0f; // could be driven externally
    globals.masterVolume = masterVolume_;
    globals.subLevel     = subLevel_;
    globals.frameCount   = static_cast<uint32_t>(n);
    globals.voiceCount   = static_cast<uint32_t>(polyphony_);

    if (globalBuf_[bufIndex]) {
        std::memcpy([globalBuf_[bufIndex] contents],
                    &globals,
                    sizeof(GlobalGPUParams));
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
    [cmd commit];
    [cmd waitUntilCompleted];

    float *out = (float *)[outBuf_[bufIndex] contents];
    for (int f = 0; f < n; ++f) {
        L[f] = out[f * 2 + 0];
        R[f] = out[f * 2 + 1];
    }

    int nextIdx = (bufIndex + 1) % 3;
    current_.store(nextIdx, std::memory_order_release);
}

void JunoRenderEngine::shutdown() {
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
}
