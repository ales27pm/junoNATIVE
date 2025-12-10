// ============================================================

// This version of JunoEngine wires in the new:
//   * Juno106::PatchParser / ParameterScaler
//   * BBDChorus
//   * HPF step handling
//
// It assumes the following types already exist in your project:
//
//   struct VoiceParams {  // as in the earlier scaffold
//       float cutoffHz;
//       float resonance;
//       float envToFilter;
//       float lfoToFilter;
//       float lfoRateHz;
//       float pwmDepth;
//       float envAttack;
//       float envDecay;
//       float envSustain;
//       float envRelease;
//       float dcoBeating;
//       float filterDrift;
//       float envelopeClick;
//       float filterTemp;
//       float filterAge;
//   };
//
//   class JunoVoice {
//   public:
//       void init(int index, float sr);
//       void setParams(const VoiceParams&);
//       void noteOn(int midiNote, float velocity);
//       void noteOff();
//       bool isActive() const;
//       float envelopeLevel() const;
//       float processSample();
//       void setAftertouch(float pressure);
//       int  getCurrentNote() const;
//   };
//
//   class BBDClockNoise { ... };
//   class PowerSupplySag { ... };
//   class CableCapacitance { ... };

#pragma once

#include <array>
#include <mutex>

// These includes refer to your existing files:
#include "dsp/BBDClockNoise.hpp"
#include "dsp/PowerSupplySag.hpp"
#include "dsp/CableCapacitance.hpp"
#include "dsp/BBDChorus.hpp"
#include "parser/Juno106PatchParser.hpp"
#include "dsp/ParameterScaler.hpp"
#include "dsp/JunoVoice.hpp"

class JunoEngine {
public:
    static constexpr int VOICE_COUNT = 6;

    void init(double sr) {
        std::lock_guard<std::mutex> g(_mtx);
        _sr = static_cast<float>(sr);
        for (int i = 0; i < VOICE_COUNT; ++i) {
            _voices[i].init(i, _sr);
            _voiceParams[i] = _defaultVoiceParams;
            _voices[i].setParams(_voiceParams[i]);
        }
        _bbdNoise.setSampleRate(_sr);
        _bbdNoise.setClockRate(15000.0f);
        _powerSag.setSampleRate(_sr);
        _cableSim.setCableLength(3.0f, _sr);
        _chorus.setSampleRate(_sr);
        _chorus.setMode(BBDChorus::Mode::Off);
    }

    void setVoiceParams(const VoiceParams& p) {
        std::lock_guard<std::mutex> g(_mtx);
        _defaultVoiceParams = p;
        for (int i = 0; i < VOICE_COUNT; ++i) {
            _voiceParams[i] = p;
            _voices[i].setParams(_voiceParams[i]);
        }
    }

    void setAnalogCharacter(float dcoBeating,
                            float filterDrift,
                            float envClick,
                            float cableLength,
                            float temperature,
                            float age) {
        std::lock_guard<std::mutex> g(_mtx);
        _defaultVoiceParams.dcoBeating    = dcoBeating;
        _defaultVoiceParams.filterDrift   = filterDrift;
        _defaultVoiceParams.envelopeClick = envClick;
        _defaultVoiceParams.filterTemp    = temperature;
        _defaultVoiceParams.filterAge     = age;
        _cableSim.setCableLength(cableLength, _sr);
        for (int i = 0; i < VOICE_COUNT; ++i) {
            _voiceParams[i] = _defaultVoiceParams;
            _voices[i].setParams(_voiceParams[i]);
        }
    }

    // 0 = Off, 1 = Chorus I, 2 = Chorus II
    void setChorusMode(int mode) {
        std::lock_guard<std::mutex> g(_mtx);
        switch (mode) {
            case 1: _chorus.setMode(BBDChorus::Mode::I);  break;
            case 2: _chorus.setMode(BBDChorus::Mode::II); break;
            default: _chorus.setMode(BBDChorus::Mode::Off); break;
        }
    }

    // Load a single Juno‑106 patch from raw sysex and apply it to the global voice params.
    // Returns true on success, false on parse/validation failure.
    bool loadJuno106Sysex(const uint8_t* data, size_t length) {
        if (!data || length == 0) return false;
        std::vector<uint8_t> buf(data, data + length);
        std::vector<Juno106::JunoPatch> patches;
        try {
            patches = Juno106::PatchParser::parseBuffer(buf, 0);
        } catch (...) {
            return false;
        }
        if (patches.empty()) return false;
        applyPatch(patches[0]);
        return true;
    }

    void noteOn(int midiNote, float velocity) {
        std::lock_guard<std::mutex> g(_mtx);
        _currentNote = midiNote;
        int voiceIdx = findVoiceToSteal();
        _voices[voiceIdx].setParams(_voiceParams[voiceIdx]);
        _voices[voiceIdx].noteOn(midiNote, velocity);
    }

    void noteOff(int midiNote) {
        std::lock_guard<std::mutex> g(_mtx);
        for (int i = 0; i < VOICE_COUNT; ++i) {
            if (_voices[i].getCurrentNote() == midiNote) {
                _voices[i].noteOff();
            }
        }
    }

    void setPolyAftertouch(int voiceIndex, float pressure) {
        if (voiceIndex < 0 || voiceIndex >= VOICE_COUNT) return;
        std::lock_guard<std::mutex> g(_mtx);
        _voices[voiceIndex].setAftertouch(pressure);
    }

    void render(float* outL, float* outR, int frames) {
        std::lock_guard<std::mutex> g(_mtx);

        for (int f = 0; f < frames; ++f) {
            float mix = 0.0f;
            int activeVoices = 0;
            float totalRes = 0.0f;

            for (int i = 0; i < VOICE_COUNT; ++i) {
                if (_voices[i].isActive()) {
                    activeVoices++;
                    totalRes += _voiceParams[i].resonance;
                }
            }

            _powerSag.update(activeVoices, totalRes);

            for (int i = 0; i < VOICE_COUNT; ++i) {
                float s = _voices[i].processSample();
                mix += s;
            }

            float noise = _bbdNoise.process();
            mix += noise * 0.2f;

            mix *= _powerSag.outputComp();
            float cabled = _cableSim.process(mix);

            float l = 0.0f;
            float r = 0.0f;
            if (_chorus.mode() == BBDChorus::Mode::Off) {
                l = cabled;
                r = cabled;
            } else {
                _chorus.process(cabled, l, r);
            }

            outL[f] = l * 0.7f;
            outR[f] = r * 0.7f;
        }
    }

private:
    int findVoiceToSteal() const {
        int idx = 0;
        float minLevel = 1e9f;
        for (int i = 0; i < VOICE_COUNT; ++i) {
            float lvl = _voices[i].envelopeLevel();
            if (lvl < minLevel) {
                minLevel = lvl;
                idx = i;
            }
        }
        return idx;
    }

    void applyPatch(const Juno106::JunoPatch& p) {
        VoiceParams vp = _defaultVoiceParams;

        // Map VCF
        vp.cutoffHz   = Juno106::ParameterScaler::vcfCutoffToHz(p.vcfCutoff);
        vp.resonance  = static_cast<float>(p.vcfResonance) / 127.0f;

        // Envelope to filter amount (centered around 0.5 so negative values
        // can be used when vcfEnvPositive is false)
        const float envAmount = static_cast<float>(p.vcfEnvMod) / 127.0f;
        vp.envToFilter = p.switches.vcfEnvPositive ? envAmount : -envAmount;

        // LFO to filter
        vp.lfoToFilter = static_cast<float>(p.vcfLfoMod) / 127.0f;
        vp.lfoRateHz   = Juno106::ParameterScaler::lfoRateToHz(p.lfoRate);

        // PWM depth: map 0‑127 to something musically useful (0–1 range)
        vp.pwmDepth = static_cast<float>(p.dcoPwmDepth) / 127.0f;

        // Envelope times
        vp.envAttack  = Juno106::ParameterScaler::envelopeTimeToSeconds(p.envAttack, true);
        vp.envDecay   = Juno106::ParameterScaler::envelopeTimeToSeconds(p.envDecay, false);
        vp.envRelease = Juno106::ParameterScaler::envelopeTimeToSeconds(p.envRelease, false);
        vp.envSustain = static_cast<float>(p.envSustain) / 127.0f;

        // Simple mapping for analog character / drift; these can later be
        // refined to depend on patch age, chorus usage, etc.
        vp.dcoBeating    = 0.5f + 0.5f * (static_cast<float>(p.dcoLfoMod) / 127.0f);
        vp.filterDrift   = 0.5f;
        vp.envelopeClick = 0.5f;
        vp.filterTemp    = 0.5f;
        vp.filterAge     = 0.3f;

        // HPF steps: 0=off, 1=~80 Hz, 2=~160 Hz, 3=~360 Hz.
        _currentHPFStep = p.switches.hpfSetting;

        _defaultVoiceParams = vp;
        for (int i = 0; i < VOICE_COUNT; ++i) {
            _voiceParams[i] = vp;
            _voices[i].setParams(_voiceParams[i]);
        }

        // Chorus mode from patch
        if (!p.switches.chorusOn) {
            setChorusMode(0);
        } else if (p.switches.chorusLevelII) {
            setChorusMode(2);
        } else {
            setChorusMode(1);
        }
    }

    float _sr = 44100.0f;
    int   _currentNote = -1;

    std::array<JunoVoice, VOICE_COUNT>   _voices;
    std::array<VoiceParams, VOICE_COUNT> _voiceParams;
    VoiceParams _defaultVoiceParams = {
        1000.0f,   // cutoffHz
        0.2f,      // resonance
        0.5f,      // envToFilter
        0.2f,      // lfoToFilter
        4.0f,      // lfoRateHz
        0.5f,      // pwmDepth
        0.01f,     // envAttack
        0.2f,      // envDecay
        0.7f,      // envSustain
        0.4f,      // envRelease
        0.7f,      // dcoBeating
        0.8f,      // filterDrift
        0.6f,      // envelopeClick
        0.5f,      // filterTemp
        0.3f       // filterAge
    };

    // Global HPF mode for now (can be made per‑voice if desired)
    uint8_t _currentHPFStep = 0;

    BBDClockNoise    _bbdNoise;
    PowerSupplySag   _powerSag;
    CableCapacitance _cableSim;
    BBDChorus        _chorus;

    mutable std::mutex _mtx;
};
