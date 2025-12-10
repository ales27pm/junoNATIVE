#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <cstring>

class BBDChorus {
public:
    enum class Mode {
        Off = 0,
        I   = 1,
        II  = 2
    };

    BBDChorus() = default;

    void configure(float sampleRate) {
        setSampleRate(sampleRate);
    }

    void setSampleRate(float sampleRate) {
        sr_ = sampleRate;
        const float maxDelaySec = 0.050f; // 50 ms
        const std::size_t maxSamples =
            static_cast<std::size_t>(maxDelaySec * sr_) + 4;
        buffer_.assign(maxSamples, 0.0f);
        writeIndex_ = 0;
        lfoPhaseL_ = 0.0f;
        lfoPhaseR_ = 0.5f; // 180 degrees
    }

    void setMode(Mode m) { mode_ = m; }
    Mode mode() const { return mode_; }

    void reset() {
        std::fill(buffer_.begin(), buffer_.end(), 0.0f);
        writeIndex_ = 0;
        lfoPhaseL_ = 0.0f;
        lfoPhaseR_ = 0.5f;
    }

    // Process a single mono input sample and output stereo chorus.
    inline void process(float in, float &outL, float &outR) {
        if (buffer_.empty() || sr_ <= 0.0f || mode_ == Mode::Off) {
            outL = in;
            outR = in;
            return;
        }

        const float baseDelayI  = 0.012f; // 12 ms
        const float depthI      = 0.004f; // +/-4 ms
        const float baseDelayII = 0.020f; // 20 ms
        const float depthII     = 0.008f; // +/-8 ms

        const float baseDelay = (mode_ == Mode::I) ? baseDelayI : baseDelayII;
        const float depth     = (mode_ == Mode::I) ? depthI     : depthII;

        const float lfoRateL = 0.6f;  // Hz
        const float lfoRateR = 1.2f;  // Hz

        // Sine LFOs in [-1, 1]
        float lfoL = std::sin(2.0f * kPi * lfoPhaseL_);
        float lfoR = std::sin(2.0f * kPi * lfoPhaseR_);

        float delayL = baseDelay + depth * lfoL;
        float delayR = baseDelay + depth * lfoR;

        float wetL = readDelayed(delayL);
        float wetR = readDelayed(delayR);

        // Subtle noise riding on BBD clock
        float noise = noiseAmount_ * whiteNoise();

        writeSample(in + noise);

        const float dryMix = 0.7f;
        const float wetMix = 0.6f;
        outL = dryMix * in + wetMix * wetL;
        outR = dryMix * in + wetMix * wetR;

        const float invSr = 1.0f / sr_;
        lfoPhaseL_ += lfoRateL * invSr;
        lfoPhaseR_ += lfoRateR * invSr;
        if (lfoPhaseL_ >= 1.0f) lfoPhaseL_ -= 1.0f;
        if (lfoPhaseR_ >= 1.0f) lfoPhaseR_ -= 1.0f;
    }

private:
    float sr_ = 44100.0f;
    Mode  mode_ = Mode::Off;
    std::vector<float> buffer_;
    std::size_t writeIndex_ = 0;

    float lfoPhaseL_ = 0.0f;
    float lfoPhaseR_ = 0.5f;
    float noiseAmount_ = 0.003f;
    std::uint32_t noiseState_ = 0x12345678u;

    static inline constexpr float kPi = 3.14159265358979323846f;

    inline void writeSample(float x) {
        if (buffer_.empty()) return;
        buffer_[writeIndex_] = x;
        writeIndex_ = (writeIndex_ + 1) % buffer_.size();
    }

    inline float readDelayed(float delaySeconds) const {
        if (buffer_.empty()) return 0.0f;
        float delaySamples = delaySeconds * sr_;
        if (delaySamples < 0.0f) delaySamples = 0.0f;
        if (delaySamples > static_cast<float>(buffer_.size() - 2)) {
            delaySamples = static_cast<float>(buffer_.size() - 2);
        }

        float readPos = static_cast<float>(writeIndex_) - delaySamples;
        while (readPos < 0.0f) {
            readPos += static_cast<float>(buffer_.size());
        }

        std::size_t idx0 = static_cast<std::size_t>(readPos) % buffer_.size();
        std::size_t idx1 = (idx0 + 1) % buffer_.size();
        float frac = readPos - std::floor(readPos);
        float s0 = buffer_[idx0];
        float s1 = buffer_[idx1];
        return s0 + (s1 - s0) * frac;
    }

    inline float whiteNoise() {
        // xorshift32
        std::uint32_t x = noiseState_;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        noiseState_ = x;
        // convert to float -1..+1
        const std::uint32_t mant = (x & 0x007FFFFFu) | 0x3F800000u;
        float f;
        std::memcpy(&f, &mant, sizeof(float));
        return (f - 1.5f) * 2.0f;
    }
};
