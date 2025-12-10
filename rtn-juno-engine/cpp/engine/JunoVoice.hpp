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
    void advanceState(int numFrames);
    void setParam(const std::string &id, float value);
    void process(float &left, float &right);
    bool isActive() const;

    // Exposed for GPU bridge / monitoring
    float frequency_ = 0.0f;
    float velocity_  = 0.0f;

    // Lightweight getters for GPU / diagnostics
    float envelopeLevel() const { return envLevel_; }
    float phase() const { return phase_; }
    float pulseWidth() const { return pwmDepth_; }

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
    float subPhase_   = 0.0f;

    NonlinearVCF filter_;
    BBDChorus    chorus_;

    bool stepEnvelopeAndPhase();
};
