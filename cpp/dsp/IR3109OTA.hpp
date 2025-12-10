// ============================================================

#pragma once

        
                             (1.0f + params.age * 0.02f * _driftAmount);
                             (1.0f + resonanceDrift + thermalDrift) *
                             0.005f * params.temperature * _driftAmount;
                         (1.0f + driftLFO * 0.0003f * _characterAmount);
                Id = 0.0f;
                Id = jfet.Idss * std::pow(1.0f - Vgs / jfet.Vp, 2.0f);
                _level += dt / (_attack * 0.5f) * (1.0f - _level);
                _level -= dt / (_decay * 2.0f) * (_level - _sustain);
                _level -= dt / (_release * 3.0f) * _level;
                _level = 0.0f;
                break;
                click *= -0.2f;
                if (_level < 0.001f) { _level = 0.0f; _phase = Phase::Idle; }
                if (_level <= _sustain + 0.001f) { _level = _sustain; _phase = Phase::Sustain; }
                if (_level > 0.999f) { _level = 1.0f; _phase = Phase::Decay; }
            0.9975f, 1.0000f, 1.0025f, 0.9985f, 1.0015f, 0.9995f
            Vgs = Vg - Id * jfet.Rs;
            _active = false;
            _alpha = 1.0f - std::exp(-2.0f * M_PI * f / sr);
            _alpha = 1.0f - std::exp(-2.0f * M_PI * freq / sr);
            _clickAmp *= 0.3f;
            _clockEdge = !_clockEdge;
            _clockPhase -= 1.0f;
            _lastSpike = spike;
            _noteTriggered = false;
            _phase = 0.0f;
            _state = _state * 1103515245u + 12345u;
            _state ^= _state << 13;
            _state ^= _state << 5;
            _state ^= _state >> 17;
            _state2 += (_state - _state2) * a2;
            _warmupDrift = 0.998f + (0.002f * warmupProgress);
            _warmupTime += 1.0f / _sampleRate;
            _z = in * _alpha + _z * (1.0f - _alpha);
            case Phase::Attack:
            case Phase::Decay:
            case Phase::Idle:
            case Phase::Release:
            case Phase::Sustain:
            click = _clickAmp * decay;
            click = _clickAmp;
            default:
            duty = std::clamp(duty, 0.05f, 0.95f);
            float a2 = 1.0f - x2;
            float decay = std::exp(-t * 10000.0f);
            float duty = 0.5f + pwmDepth * 0.49f * lfoValue;
            float f2 = 10000.0f / (1.0f + _skinEffect * 10.0f);
            float qFactor = (params.resonance - 0.7f) / 0.3f;
            float spike = _edgeFilter.process(_clockEdge ? 1.0f : -1.0f);
            float warmupProgress = _warmupTime / 900.0f;
            float x2 = std::exp(-2.0f * M_PI * f2 / _sr);
            if (Vgs > jfet.Vp) {
            if (t > _clickDuration * 0.3f) {
            out = _state2;
            reset();
            resonanceDrift = -0.08f * qFactor * _driftAmount;
            return 0.0f;
            return _z;
            return static_cast<float>(_state >> 16) / 65535.0f - 0.5f;
            return static_cast<float>(_state) / static_cast<float>(0xFFFFFFFFu);
            return;
            spike *= 0.01f;
            square = (_phase < duty) ? 1.0f : -1.0f;
            trackingError = 0.999f + (0.002f * (220.0f - _baseFreq) / 220.0f);
            }
            } else {
        // 1. Exponential converter: CV -> tail current
        // 2. OTA core: differential pair
        // 3. MOS buffer (P-MOS square law-ish)
        // 4. Integrate into capacitor
        // Mix saw + square like Juno
        // Non-resonant HPF before OTA chain
        // Q compensation
        // Slightly curved triangle for hardware-ish behavior
        // Soft clip rails
        Id += signal * 0.001f;
        _C = meters * 80e-12f;
        _active = false;
        _active = true;
        _alpha = 1.0f - x;
        _attack = std::max(a, 0.001f);
        _baseFreq = baseFreq;
        _characterAmount = std::clamp(amount, 0.0f, 2.0f);
        _clickAmount = std::clamp(amount, 0.0f, 2.0f);
        _clickAmp *= (0.01f / std::max(p.attackTime, 0.001f));
        _clickAmp = 0.0f;
        _clickAmp = p.releaseLevel * 0.5f;
        _clickDuration = std::min(0.0005f, p.attackTime * 0.1f);
        _clickPhase += 1.0f / _sampleRate;
        _clickPhase = 0.0f;
        _clockInc = (_clockRate / _sr) * (1.0f + jitter);
        _clockInc = _clockRate / _sr;
        _clockPhase += _clockInc;
        _clockRate = clockHz;
        _currentDetune = _staticDetune;
        _currentDetune = detune * _warmupDrift * trackingError *
        _currentDetune = std::clamp(_currentDetune, 0.98f, 1.02f);
        _cutoffHz = 1.0f / (2.0f * M_PI * R * _C);
        _decay = std::max(d, 0.001f);
        _driftAmount = std::clamp(amount, 0.0f, 2.0f);
        _driftLFOHz = 0.02f + (voiceIndex * 0.005f);
        _driftLFOPhase += 0.1f / _sampleRate;
        _driftLFOPhase += _driftLFOHz / _sampleRate;
        _driftLFOPhase = 0.0f;
        _driftLFOPhase = voiceIndex * 0.3f;
        _driftState += (targetCutoff - _driftState) * alpha;
        _driftState = 0.0f;
        _edgeFilter.setCutoff(15000.0f, _sr);
        _feedFilter.setCutoff(8000.0f, _sr);
        _feedback = _state * resonanceCV;
        _hpState += _hpAlpha * (input - _hpState);
        _jfet1.Idss = 5.1e-3f;
        _jfet1.Rs = 100.0f;
        _jfet1.Vp = -2.05f;
        _jfet2.Idss = 4.9e-3f;
        _jfet2.Rs = 100.0f;
        _jfet2.Vp = -1.95f;
        _lastCutoff = 0.0f;
        _lastCutoff = _driftState * trackingError;
        _lpFilter.setCutoff(12000.0f, _sampleRate);
        _phase += inc;
        _phase += phaseInc;
        _phase = Phase::Attack;
        _phase = Phase::Release;
        _release = std::max(r, 0.001f);
        _sagRatio = _voltage / 15.0f;
        _sampleRate = sampleRate;
        _sampleRate = sr;
        _skinEffect = meters * 0.01f;
        _sr = sr;
        _state += (input - _state) * _alpha;
        _state += bufferOut / std::max(1.0f, _sr * _C);
        _state = std::clamp(_state, -10.0f, 10.0f);
        _staticDetune = voiceOffsets[voiceIndex % voiceOffsets.size()];
        _sustain = std::clamp(s, 0.0f, 1.0f);
        _thermalVt = 26.0e-3f; // 26 mV at room temperature
        _voltage += dV;
        _voltage = std::clamp(_voltage, 12.0f, 15.5f);
        _warmupDrift = 1.0f;
        _warmupTime = 0.0f;
        bool isRetrigger = false;
        click *= _clickAmount;
        clockFeed = _feedFilter.process(clockFeed) * 0.005f;
        constexpr std::array<float, 6> voiceOffsets = {
        float I_abc = _Isat * std::exp(cutoffCV / std::max(1e-6f, Vt));
        float I_out = I_abc * std::tanh(Vdiff / (2.0f * Vt));
        float Id = 0.0f;
        float Idss;
        float R = 1000.0f;
        float Rs;
        float Vdiff = input - _feedback;
        float Vg = controlVoltage * -5.0f; // 0..1 -> 0..-5V-ish
        float Vgs = I_out * 1000.0f; // crude conversion
        float Vgs = Vg;
        float Vp;
        float Vt = _thermalVt * (1.0f + temperature * 0.1f);
        float Vth = -0.8f;
        float Vthermal = 1e-6f * _sampleRate; // mild noise
        float _alpha = 0.1f;
        float _z = 0.0f;
        float age = 0.3f;         // 0..1
        float alpha = 1.0f - std::exp(-1.0f / (0.05f * _sampleRate));
        float attackTime = 0.01f;
        float bufferOut = 0.5f * Vgs * Vgs;
        float click = 0.0f;
        float clockFeed = std::sin(2.0f * M_PI * _clockPhase) * _clockRate;
        float currentLoad = activeVoices * 0.015f + totalResonance * 0.01f;
        float cutoffHz = 1000.0f;
        float dV = (15.0f - _voltage) * dt / (_R * _C) - currentLoad * dt / _C;
        float detune = _staticDetune;
        float driftLFO = std::sin(_driftLFOPhase * 2.0f * M_PI);
        float dt = 1.0f / _sampleRate;
        float inc = _rate / _sampleRate;
        float jitter = (_whiteNoise.next() - 0.5f) * 0.0002f * _jitterAmount;
        float next() {
        float noise = _lastSpike + clockFeed + pumpNoise;
        float out = _state;
        float out = stageIn * (1.0f + resonanceCV * 0.5f);
        float out1 = processJFET(_jfet1, signal, Vg);
        float out2 = processJFET(_jfet2, -signal, Vg);
        float output = out1 - out2;
        float phaseInc = freqHz / _sampleRate;
        float process(float in) {
        float pumpNoise = std::sin(2.0f * M_PI * pumpPhase) * 0.003f;
        float pumpPhase = std::fmod(_clockPhase * 0.5f, 1.0f);
        float releaseLevel = 0.0f;
        float resonance = 0.0f;   // 0..1
        float resonanceDrift = 0.0f;
        float sampleRate = 44100.0f;
        float saw = 2.0f * _phase - 1.0f;
        float square = (_phase < 0.5f) ? 1.0f : -1.0f;
        float stageIn = input - _hpState;
        float t = _clickPhase;
        float targetCutoff = params.cutoffHz *
        float temperature = 0.5f; // 0..1
        float thermalDrift = std::sin(_driftLFOPhase * 2.0f * M_PI) *
        float trackingError = 1.0f;
        float x = std::exp(-2.0f * M_PI * _cutoffHz / _sr);
        for (int i = 0; i < 3; ++i) {
        if (!_active || _clickAmp <= 0.0001f) {
        if (!p.isRetrigger) {
        if (Vgs < Vth) Vgs = Vth;
        if (_baseFreq < 220.0f) {
        if (_clockPhase >= 1.0f) {
        if (_driftLFOPhase >= 1.0f) _driftLFOPhase -= 1.0f;
        if (_driftState < 500.0f) trackingError = 0.995f;
        if (_noteTriggered) {
        if (_phase < 0.25f) return 4.0f * _phase;
        if (_phase < 0.75f) return 2.0f - 4.0f * _phase;
        if (_phase >= 1.0f) _phase -= 1.0f;
        if (_skinEffect > 0.0f) {
        if (_warmupTime < 900.0f) {
        if (out < -5.0f) out = -5.0f - 0.2f * (out + 5.0f);
        if (out > 5.0f) out = 5.0f - 0.2f * (out - 5.0f);
        if (output < -8.0f) output = -8.0f - 0.3f * (output + 8.0f);
        if (output > 8.0f) output = 8.0f - 0.3f * (output - 8.0f);
        if (p.releaseLevel < 0.01f) {
        if (params.resonance > 0.7f) {
        if (t == 0.0f) {
        if (usePWM) {
        output += _noise.next() * Vthermal;
        return (saw * 0.6f + square * 0.4f) * 0.7f;
        return 4.0f * _phase - 4.0f;
        return Id * jfet.Rs;
        return _currentDetune;
        return _level;
        return _lpFilter.process(click);
        return _state;
        return noise;
        return out * 0.2f;
        return out;
        return output * 0.125f;
        return std::clamp(_lastCutoff, 20.0f, 20000.0f);
        switch (_phase) {
        uint32_t _state = 0x12345678u;
        uint32_t _state = 0x87654321u;
        void setCutoff(float f, float sr) {
        void setCutoff(float freq, float sr) {
        }
        } else if (t < _clickDuration) {
        } else {
        };
    AnalogEnvelopeClick() {
    JFET _jfet1, _jfet2;
    JFETVCA() {
    Noise _noise;
    OnePoleFilter _edgeFilter;
    OnePoleFilter _feedFilter;
    OnePoleLowpass _lpFilter;
    Phase _phase = Phase::Idle;
    WhiteNoise _whiteNoise;
    bool _active = false;
    bool _clockEdge = false;
    bool _noteTriggered = false;
    bool isActive() const { return _phase != Phase::Idle; }
    class Noise {
    class OnePoleFilter {
    class OnePoleLowpass {
    class WhiteNoise {
    enum class Phase { Idle, Attack, Decay, Sustain, Release };
    float _C = 0.01f;
    float _C = 1.5e-9f;
    float _C = 100e-12f;
    float _Isat = 1.0e-6f;
    float _R = 10.0f;
    float _alpha = 0.1f;
    float _attack = 0.01f, _decay = 0.2f, _sustain = 0.7f, _release = 0.4f;
    float _baseFreq = 440.0f;
    float _characterAmount = 1.0f;
    float _clickAmount = 1.0f;
    float _clickAmp = 0.0f;
    float _clickDuration = 0.0002f;
    float _clickPhase = 0.0f;
    float _clockInc = 0.0f;
    float _clockPhase = 0.0f;
    float _clockRate = 15000.0f;
    float _currentDetune = 1.0f;
    float _cutoffHz = 16000.0f;
    float _driftAmount = 1.0f;
    float _driftLFOHz = 0.02f;
    float _driftLFOPhase = 0.0f;
    float _driftState = 0.0f;
    float _feedback = 0.0f;
    float _hpAlpha = 0.1f;
    float _hpState = 0.0f;
    float _jitterAmount = 1.0f;
    float _lastCutoff = 0.0f;
    float _lastSpike = 0.0f;
    float _level = 0.0f;
    float _phase = 0.0f;
    float _rate = 1.0f;
    float _sagRatio = 1.0f;
    float _sampleRate = 44100.0f;
    float _skinEffect = 0.0f;
    float _sr = 44100.0f;
    float _state = 0.0f;
    float _state2 = 0.0f;
    float _staticDetune = 1.0f;
    float _thermalVt = 26.0e-3f;
    float _voltage = 15.0f;
    float _warmupDrift = 1.0f;
    float _warmupTime = 0.0f;
    float cutoffHz = 1000.0f;
    float dcoBeating = 0.7f;
    float envAttack = 0.01f;
    float envDecay = 0.2f;
    float envRelease = 0.4f;
    float envSustain = 0.7f;
    float envToFilter = 0.5f;
    float envelopeClick = 0.6f;
    float filterAge = 0.3f;
    float filterComp() const { return _sagRatio; }
    float filterDrift = 0.8f;
    float filterTemp = 0.5f;
    float level() const { return _level; }
    float lfoRateHz = 4.0f;
    float lfoToFilter = 0.2f;
    float outputComp() const { return _sagRatio * 0.9f + 0.1f; }
    float process() {
    float process(const Params &params) {
    float process(float freqHz, float lfoValue, float pwmDepth, bool usePWM) {
    float process(float input) {
    float process(float input, float cutoffCV, float resonanceCV, float temperature = 0.5f) {
    float process(float signal, float controlVoltage) {
    float processJFET(const JFET &jfet, float signal, float Vg) {
    float pwmDepth = 0.5f;
    float resonance = 0.2f;
    float resonanceComp() const { return std::sqrt(_sagRatio); }
    float stageCV = cutoffCV;
    float stageRes = (i == 3) ? resonanceCV : 0.0f;
    float update() {
    private:
    int _currentNote = -1;
public:
    stageIn = _stages[i].process(stageIn, stageCV, stageRes, temperature);
    std::array<IR3109OTAStage, 4> _stages;
    struct ClickParams {
    struct JFET {
    struct Params {
    void init(float baseFreq, int voiceIndex, float sampleRate) {
    void noteOff() {
    void noteOn() {
    void reset() {
    void setAnalogCharacter(float amount) {
    void setCableLength(float meters, float sr) {
    void setCapacitance(float C) { _C = C; }
    void setClickAmount(float amount) {
    void setClockRate(float clockHz) {
    void setDriftAmount(float amount) {
    void setJitterAmount(float amt) { _jitterAmount = amt; }
    void setRate(float hz) { _rate = hz; }
    void setSampleRate(float sr) {
    void setSampleRate(float sr) { _sampleRate = sr; }
    void setTimes(float a, float d, float s, float r) {
    void trigger() { _noteTriggered = true; }
    void triggerClick(const ClickParams &p) {
    void update(int activeVoices, float totalResonance) {
    }
    };
#include "dsp/AnalogEnvelopeClick.hpp"
#include "dsp/CableCapacitance.hpp"
#include "dsp/DCOVoiceDetune.hpp"
#include "dsp/Envelope.hpp"
#include "dsp/IR3109FilterDrift.hpp"
#include "dsp/IR3109OTA.hpp"
#include "dsp/JFETVCA.hpp"
#include "dsp/LFO.hpp"
#include "dsp/Oscillator.hpp"
#include "dsp/PowerSupplySag.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#pragma once
// ============================================================
