// ============================================================

#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

class BBDChorus {
public:
    enum class Mode {
        Off = 0,
        I   = 1,
        II  = 2
    };

    void setSampleRate(float sr) {
        _sr = sr;
        const float maxDelaySec = 0.040f; // 40 ms is plenty for chorus
        const size_t maxDelaySamples = static_cast<size_t>(maxDelaySec * _sr) + 2;
        _buffer.assign(maxDelaySamples, 0.0f);
        _writeIndex = 0;
    }

    void setMode(Mode m) {
        _mode = m;
    }

    Mode mode() const { return _mode; }

    void reset() {
        std::fill(_buffer.begin(), _buffer.end(), 0.0f);
        _writeIndex = 0;
        _lfoPhaseL = 0.0f;
        _lfoPhaseR = 0.5f; // 180° out of phase
    }

    inline void process(float in, float& outL, float& outR) {
        if (_buffer.empty() || _sr <= 0.0f || _mode == Mode::Off) {
            outL = in;
            outR = in;
            return;
        }

        // Base delay/depth/ratios
        const float baseDelayI  = 0.012f;
        const float depthI      = 0.004f;
        const float baseDelayII = 0.020f;
        const float depthII     = 0.008f;

        const float baseDelay = (_mode == Mode::I) ? baseDelayI : baseDelayII;
        const float depth     = (_mode == Mode::I) ? depthI     : depthII;

        const float lfoRateL = 0.6f;  // Hz
        const float lfoRateR = 1.2f;  // Hz

        // Simple sine LFOs
        float lfoL = std::sin(2.0f * static_cast<float>(M_PI) * _lfoPhaseL);
        float lfoR = std::sin(2.0f * static_cast<float>(M_PI) * _lfoPhaseR);

        float delayL = baseDelay + depth * lfoL;
        float delayR = baseDelay + depth * lfoR;

        float wetL = readDelayed(delayL);
        float wetR = readDelayed(delayR);

        // White-ish noise riding on the BBD clock
        float noise = _noiseAmount * whiteNoise();

        // Write input + a bit of noise into the delay line
        writeSample(in + noise);

        // Mix
        const float dry  = 0.7f;
        const float wet  = 0.6f;
        outL = dry * in + wet * wetL;
        outR = dry * in + wet * wetR;

        // Advance LFO phases
        const float invSr = 1.0f / _sr;
        _lfoPhaseL += lfoRateL * invSr;
        _lfoPhaseR += lfoRateR * invSr;
        if (_lfoPhaseL >= 1.0f) _lfoPhaseL -= 1.0f;
        if (_lfoPhaseR >= 1.0f) _lfoPhaseR -= 1.0f;
    }

private:
    float _sr = 44100.0f;
    Mode _mode = Mode::Off;
    std::vector<float> _buffer;
    size_t _writeIndex = 0;

    float _lfoPhaseL = 0.0f;
    float _lfoPhaseR = 0.5f;
    float _noiseAmount = 0.003f;

    uint32_t _noiseState = 0x12345678u;

    inline void writeSample(float x) {
        if (_buffer.empty()) return;
        _buffer[_writeIndex] = x;
        _writeIndex = (_writeIndex + 1) % _buffer.size();
    }

    inline float readDelayed(float delaySeconds) const {
        if (_buffer.empty()) return 0.0f;
        float delaySamples = delaySeconds * _sr;
        if (delaySamples < 0.0f) delaySamples = 0.0f;
        if (delaySamples > static_cast<float>(_buffer.size() - 2)) {
            delaySamples = static_cast<float>(_buffer.size() - 2);
        }

        const float readPos = static_cast<float>(_writeIndex) - delaySamples;
        float idxF = readPos;
        while (idxF < 0.0f) {
            idxF += static_cast<float>(_buffer.size());
        }

        const size_t idx0 = static_cast<size_t>(idxF) % _buffer.size();
        const size_t idx1 = (idx0 + 1) % _buffer.size();
        const float frac = idxF - std::floor(idxF);
        return _buffer[idx0] + ( _buffer[idx1] - _buffer[idx0]) * frac;
    }

    inline float whiteNoise() {
        // xorshift
        _noiseState ^= (_noiseState << 13);
        _noiseState ^= (_noiseState >> 17);
        _noiseState ^= (_noiseState << 5);
        const uint32_t mant = (_noiseState & 0x007FFFFF) | 0x3F800000;
        float f;
        std::memcpy(&f, &mant, sizeof(float));
        return (f - 1.5f) * 2.0f; // roughly -1..+1
    }
};


// ============================================================
