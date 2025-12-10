#include "JunoVoice.hpp"
#include <algorithm>
#include <cmath>

void JunoVoice::initialize(float sr) {
    sampleRate_ = sr;
    filter_.configure(sr);
    chorus_.configure(sr);
    chorus_.setMode(BBDChorus::Mode::I);
}

void JunoVoice::noteOn(int midiNote, float vel) {
    active_   = true;
    velocity_ = vel;
    midiNote_ = midiNote;
    frequency_ = 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
    envLevel_  = 0.0f;
    envTarget_ = 1.0f;
    phase_     = 0.0f;
    subPhase_  = 0.0f;
}

void JunoVoice::noteOff(int midiNote) {
    if (!active_ || midiNote_ != midiNote) {
        return;
    }

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

void JunoVoice::process(float &L, float &R) {
    if (!stepEnvelopeAndPhase()) return;

    float pwm = 0.5f + (pwmDepth_ - 0.5f); // keep in 0..1
    pwm = std::clamp(pwm, 0.05f, 0.95f);

    float osc = (phase_ < pwm) ? -1.0f + (phase_ / pwm) * 2.0f
                               :  1.0f - ((phase_ - pwm) / (1.0f - pwm)) * 2.0f;

    // Sub oscillator at half frequency (square-ish)
    float sub = (subPhase_ < 0.5f ? 1.0f : -1.0f) * subLevel_;

    float mixed = osc + sub;

    // Filter
    float filtered = filter_.process(mixed, cutoff_, resonance_);

    // Chorus to stereo
    float outL = 0.0f;
    float outR = 0.0f;
    chorus_.process(filtered, outL, outR);

    L += outL * envLevel_ * velocity_;
    R += outR * envLevel_ * velocity_;
}

void JunoVoice::advanceState(int numFrames) {
    for (int i = 0; i < numFrames; ++i) {
        if (!stepEnvelopeAndPhase()) {
            return;
        }
    }
}

bool JunoVoice::stepEnvelopeAndPhase() {
    if (!active_) {
        return false;
    }

    // Envelope follower (simple one-pole towards envTarget)
    float envRate = (envTarget_ > envLevel_) ? attack_ : release_;
    if (envRate <= 0.0f || sampleRate_ <= 0.0f) {
        envLevel_ = envTarget_;
    } else {
        float step = 1.0f - std::exp(-1.0f / (envRate * sampleRate_));
        step = std::clamp(step, 0.0f, 1.0f);
        envLevel_ += (envTarget_ - envLevel_) * step;
    }

    if (envLevel_ < 1e-4f && envTarget_ == 0.0f) {
        active_ = false;
        midiNote_ = -1;
        return false;
    }

    // Simple sawtooth oscillator with PWM-like modulation
    phase_ += frequency_ / std::max(sampleRate_, 1.0f);
    if (phase_ >= 1.0f) phase_ -= 1.0f;

    subPhase_ += (frequency_ * 0.5f) / std::max(sampleRate_, 1.0f);
    if (subPhase_ >= 1.0f) subPhase_ -= 1.0f;

    return true;
}
