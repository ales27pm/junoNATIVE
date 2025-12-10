#pragma once
#include "JunoVoice.hpp"
#include "RCUParameterManager.hpp"
#include <vector>
#include <memory>
#include <string>
#include <atomic>

class JunoRenderEngine; // Forward declaration
struct VoiceGPUParams;

namespace Juno106 {
    struct JunoPatch; // Real definition lives in parser header
}

class JunoDSPEngine {
public:
    bool initialize(int sampleRate, int bufferSize, int polyphony, bool useGPU);
    void start();
    void stop();

    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);

    void setParameter(const std::string &id, float value);
    void loadPatch(const Juno106::JunoPatch &patch);

    void renderAudio(float *left, float *right, int numFrames);

private:
    std::vector<std::unique_ptr<JunoVoice>> voices_;
    RCUParameterManager params_;
    int  sampleRate_ = 44100;
    int  bufferSize_ = 256;
    std::atomic<bool> running_{false};
    bool useGPU_     = false;

    std::unique_ptr<JunoRenderEngine> gpu_;
    std::unique_ptr<std::vector<VoiceGPUParams>> gpuVoiceCache_;
};
