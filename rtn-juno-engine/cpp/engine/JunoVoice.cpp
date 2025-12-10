#include "JunoVoice.hpp"

void JunoVoice::initialize(float sr) {
    sampleRate_ = sr;
    filter_.configure(sr);
    chorus_.configure(sr);
    chorus_.setMode(BBDChorus::Mode::I);
}

void JunoVoice::noteOn(int midiNote, float vel) {
    active_   = true;
    velocity_ = vel;
    frequency_ = 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
    envLevel_  = 0.0f;
    envTarget_ = 1.0f;
    phase_     = 0.0f;
    subPhase_  = 0.0f;
    midiNote_  = midiNote;
}

void JunoVoice::noteOff(int midiNote) {
    if (midiNote_ != midiNote) return;
    // Let envelope decay
    envTarget_ = 0.0f;
}

bool JunoVoice::isActive() const {
    return active_;
}

void JunoVoice::setParam(const std::string &id, float v) {
    if (id == "cutoff") {
        cutoff_ = v;
    } else if (id == "resonance") {
        resonance_ = v;
    } else if (id == "attack") {
        attack_ = std::max(0.0005f, v);
    } else if (id == "release") {
        release_ = std::max(0.0005f, v);
    } else if (id == "pwmDepth") {
        pwmDepth_ = v;
    } else if (id == "subLevel") {
        subLevel_ = v;
    }
}

void JunoVoice::processBlock(float *L, float *R, int numFrames) {
    if (!active_ || !L || !R || numFrames <= 0) return;

    const float freqInc = (sampleRate_ > 0.0f) ? (frequency_ / sampleRate_) : 0.0f;
    const float subFreqInc = freqInc * 0.5f;

    const float envTime  = (envTarget_ > envLevel_) ? attack_ : release_;
    const float envDenom = std::max(envTime * sampleRate_, 1.0f);
    const float envCoeff = std::exp(-1.0f / envDenom);

    for (int i = 0; i < numFrames; ++i) {
        envLevel_ = envTarget_ + (envLevel_ - envTarget_) * envCoeff;

        if (envLevel_ < 1e-4f && envTarget_ == 0.0f) {
            active_ = false;
            midiNote_ = -1;
            break;
        }

        // Simple sawtooth oscillator with PWM-like modulation
        phase_ += freqInc;
        if (phase_ >= 1.0f) phase_ -= 1.0f;

        subPhase_ += subFreqInc;
        if (subPhase_ >= 1.0f) subPhase_ -= std::floor(subPhase_);

        float pwm = std::clamp(pwmDepth_, 0.05f, 0.95f);

        float osc = (phase_ < pwm) ? -1.0f + (phase_ / pwm) * 2.0f
                                   :  1.0f - ((phase_ - pwm) / (1.0f - pwm)) * 2.0f;

        float sub = (subPhase_ < 0.5f ? 1.0f : -1.0f) * subLevel_;

        float mixed = osc + sub;

        float filtered = filter_.process(mixed, cutoff_, resonance_);

        float outL = 0.0f;
        float outR = 0.0f;
        chorus_.process(filtered, outL, outR);

        L[i] += outL * envLevel_ * velocity_;
        R[i] += outR * envLevel_ * velocity_;
    }
}
