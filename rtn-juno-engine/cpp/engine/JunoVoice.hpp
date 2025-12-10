#pragma once
#include "../dsp/NonlinearVCF.hpp"
#include "../dsp/BBDChorus.hpp"
#include <cmath>
#include <string>

class JunoVoice {
public:
    void initialize(float sampleRate);
    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);
    void setParam(const std::string &id, float value);
    void processBlock(float *left, float *right, int numFrames);
    bool isActive() const;
    int midiNote() const { return midiNote_; }

    // Exposed for GPU bridge / monitoring
    float frequency_ = 0.0f;
    float velocity_  = 0.0f;

private:
    float sampleRate_ = 44100.0f;
    float phase_      = 0.0f;
    float envLevel_   = 0.0f;
    float envTarget_  = 0.0f;
    bool  active_     = false;
    int   midiNote_   = -1;

    float attack_     = 0.01f;
    float release_    = 0.5f;
    float cutoff_     = 1000.0f;
    float resonance_  = 0.1f;
    float subLevel_   = 0.0f;
    float pwmDepth_   = 0.5f;

    NonlinearVCF filter_;
    BBDChorus    chorus_;
};
