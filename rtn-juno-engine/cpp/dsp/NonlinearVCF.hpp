#pragma once
#include <cmath>
#include <algorithm>

class NonlinearVCF {
public:
    NonlinearVCF() = default;

    void configure(float sampleRate) {
        sampleRate_ = sampleRate;
        reset();
    }

    void reset() {
        for (auto &s : stage_) s = 0.0f;
    }

    // Simple 4-pole low-pass ladder-style filter with soft saturation.
    // cutoffHz:   20–20000
    // resonance:  0.0–1.0 (self-oscillation near 1.0)
    float process(float input, float cutoffHz, float resonance) {
        if (sampleRate_ <= 0.0f) return input;

        cutoffHz = std::clamp(cutoffHz, 20.0f, sampleRate_ * 0.45f);
        resonance = std::clamp(resonance, 0.0f, 1.2f);

        const float fc = cutoffHz / sampleRate_;
        // bilinear transform approx for one-pole
        const float x = std::exp(-2.0f * kPi * fc);
        const float g = 1.0f - x;

        // Simple feedback from last stage
        float fb = resonance * 3.5f; // tuned so that self-osc near 1.0
        float x_in = softClip(input - fb * stage_[3]);

        for (int i = 0; i < 4; ++i) {
            stage_[i] = stage_[i] + g * (x_in - stage_[i]);
            x_in = stage_[i];
        }

        return softClip(stage_[3]);
    }

private:
    float sampleRate_ = 44100.0f;
    float stage_[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    static inline float softClip(float x) {
        // tanh-style soft clip
        return std::tanh(x * 1.5f);
    }

    static inline constexpr float kPi = 3.14159265358979323846f;
};
