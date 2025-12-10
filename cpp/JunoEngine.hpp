// ============================================================

#pragma once
#include "JunoVoice.hpp"
#include "dsp/BBDClockNoise.hpp"
#include "dsp/PowerSupplySag.hpp"
#include "dsp/CableCapacitance.hpp"
#include <array>
#include <mutex>

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
    }

    void setVoiceParams(const VoiceParams &p) {
        std::lock_guard<std::mutex> g(_mtx);
        _defaultVoiceParams = p;
        for (int i = 0; i < VOICE_COUNT; ++i) {
            _voiceParams[i] = p;
            _voices[i].setParams(_voiceParams[i]);
        }
    }

    void setAnalogCharacter(float dcoBeating, float filterDrift, float envClick,
                            float cableLength, float temperature, float age) {
        std::lock_guard<std::mutex> g(_mtx);
        _defaultVoiceParams.dcoBeating = dcoBeating;
        _defaultVoiceParams.filterDrift = filterDrift;
        _defaultVoiceParams.envelopeClick = envClick;
        _defaultVoiceParams.filterTemp = temperature;
        _defaultVoiceParams.filterAge = age;
        _cableSim.setCableLength(cableLength, _sr);
        for (int i = 0; i < VOICE_COUNT; ++i) {
            _voiceParams[i] = _defaultVoiceParams;
            _voices[i].setParams(_voiceParams[i]);
        }
    }

    void noteOn(int midiNote, float velocity) {
        _currentNote = midiNote;
        std::lock_guard<std::mutex> g(_mtx);
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

    void render(float *outL, float *outR, int frames) {
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

            outL[f] = cabled * 0.7f;
            outR[f] = cabled * 0.7f;
        }
    }

private:
    int _currentNote = -1;
int findVoiceToSteal() {
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

    float _sr = 44100.0f;
    std::array<JunoVoice, VOICE_COUNT> _voices;
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

    BBDClockNoise _bbdNoise;
    PowerSupplySag _powerSag;
    CableCapacitance _cableSim;

    std::mutex _mtx;
};


// ============================================================
