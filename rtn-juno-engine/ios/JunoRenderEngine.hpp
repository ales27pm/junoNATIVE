#pragma once
#include <Metal/Metal.h>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

// GPU-side per-voice parameters. This is intentionally minimal so
// it can be filled from JunoVoice state without tight coupling.
struct VoiceGPUParams {
    float frequency;
    float velocity;
    float envelope;
    float phase[3];
    float pulseWidth;
    uint32_t active; // use uint32_t for better alignment in Metal
};

struct GlobalGPUParams {
    float sampleRate;
    float lfoValue;
    float masterVolume;
    float subLevel;
    uint32_t frameCount;
    uint32_t voiceCount;
    uint32_t reserved0;
    uint32_t reserved1;
};

class JunoRenderEngine {
public:
    JunoRenderEngine();
    bool initialize(float sampleRate, int polyphony, int maxFramesPerBuffer);
    void updateVoices(const std::vector<VoiceGPUParams> &voices);
    void render(float *outL, float *outR, int numFrames);
    void shutdown();

private:
    id<MTLDevice> device_ = nil;
    id<MTLCommandQueue> queue_ = nil;
    id<MTLLibrary> lib_ = nil;
    id<MTLComputePipelineState> pipeline_ = nil;

    id<MTLBuffer> voiceBuf_[3]  = {nil, nil, nil};
    id<MTLBuffer> globalBuf_[3] = {nil, nil, nil};
    id<MTLBuffer> outBuf_[3]    = {nil, nil, nil};

    std::atomic<int> current_;
    std::mutex lock_;
    int polyphony_ = 8;
    int maxFrames_ = 512;
    float sampleRate_ = 44100.0f;
    float masterVolume_ = 0.8f;
    float subLevel_ = 0.0f;
    bool init_ = false;
};
