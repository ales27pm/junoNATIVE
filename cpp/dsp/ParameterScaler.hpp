// ============================================================

#pragma once

#include <cstdint>
#include <cmath>

namespace Juno106 {

class ParameterScaler {
public:
    // JUNO‑style cutoff: 50 Hz → ~15 kHz with a log curve.
    static float vcfCutoffToHz(uint8_t midiValue,
                               float minFreq = 50.0f,
                               float maxFreq = 15000.0f) {
        const float t = static_cast<float>(midiValue) / 127.0f;
        const float logMin = std::log(minFreq);
        const float logMax = std::log(maxFreq);
        const float logF = logMin + (logMax - logMin) * t;
        return std::exp(logF);
    }

    // Envelope time sliders are approximately exponential in time.
    // Attack: 1.5 ms → 3 s
    // Decay/Release: 1.5 ms → 12 s
    static float envelopeTimeToSeconds(uint8_t midiValue, bool isAttack = false) {
        const float t = static_cast<float>(midiValue) / 127.0f;
        const float minT = 0.0015f;
        const float maxT = isAttack ? 3.0f : 12.0f;
        const float logMin = std::log(minT);
        const float logMax = std::log(maxT);
        const float logV = logMin + (logMax - logMin) * t;
        return std::exp(logV);
    }

    // LFO rate: about 0.5–30 Hz with a tilted exponential curve.
    static float lfoRateToHz(uint8_t midiValue) {
        const float t = static_cast<float>(midiValue) / 127.0f;
        const float minHz = 0.5f;
        const float maxHz = 30.0f;
        const float logMin = std::log(minHz);
        const float logMax = std::log(maxHz);
        const float logV = logMin + (logMax - logMin) * t;
        return std::exp(logV);
    }
};

} // namespace Juno106


// ============================================================
