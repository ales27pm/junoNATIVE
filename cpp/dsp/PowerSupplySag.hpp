class AnalogEnvelopeClick {
class BBDClockNoise {
class CableCapacitance {
class DCO {
class DCOVoiceDetune {
class ExponentialADSR {
class IR3109Filter {
class IR3109FilterDrift {
class IR3109OTAStage {
class JFETVCA {
class JunoLFO {
class PowerSupplySag {
for (int i = 0; i < 4; ++i) {
private:
    int _currentNote = -1;
public:
struct VoiceParams {
}
};class JunoVoice {
public:
    int getCurrentNote() const { return _currentNote; }
void init(int index, float sr) {
        _index = index;
        _sr = sr;
        _filter.setSampleRate(sr);
        _filterDrift.setSampleRate(sr);
        _env.setSampleRate(sr);
        _dco.setSampleRate(sr);
        _lfo.setSampleRate(sr);
        _vca.setSampleRate(sr);
        _click.setSampleRate(sr);
    }

    void setParams(const VoiceParams &p) { _params = p; }

    void noteOn(int midiNote, float velocity) {
        _currentNote = midiNote;
        float newFreq = midiToFreq(midiNote);
        bool retrigger = _env.isActive();

        _baseFreq = newFreq;
        _detune.init(_baseFreq, _index, _sr);
        _detune.setAnalogCharacter(_params.dcoBeating);
        _filterDrift.setDriftAmount(_params.filterDrift);
        _click.setClickAmount(_params.envelopeClick);

        if (retrigger) {
            AnalogEnvelopeClick::ClickParams cp;
            cp.isRetrigger = true;
            cp.releaseLevel = _env.level();
            cp.attackTime = _params.envAttack;
            cp.sampleRate = _sr;
            _click.triggerClick(cp);
        }

        _env.setTimes(_params.envAttack, _params.envDecay,
                      _params.envSustain, _params.envRelease);
        _env.noteOn();
        _lfo.trigger();
        _active = true;
        _age = 0.0;
    }

    void noteOff() {
        _env.noteOff();
    }

    bool isActive() const { return _env.isActive(); }
    float envelopeLevel() const { return _env.level(); }

    void setAftertouch(float pressure) { _aftertouch = pressure; }

    float processSample() {
        if (!_active && !_env.isActive()) {
            return 0.0f;
        }
        _age += 1.0 / _sr;

        float envVal = _env.process();
        float lfoVal = _lfo.process();
        float click = _click.process();

        float det = _detune.update();
        float freq = _baseFreq * det;
        float osc = _dco.process(freq, lfoVal, _params.pwmDepth, true);
        osc += click * 0.7f;

        // Compute cutoff with mods + drift
        float cutoffNorm = std::clamp(std::log10(_params.cutoffHz) / 4.0f, 0.0f, 1.0f);
        float envMod = envVal * _params.envToFilter;
        float lfoMod = lfoVal * _params.lfoToFilter;
        float atMod = _aftertouch * 0.5f;
        float cutoffMod = cutoffNorm + envMod + lfoMod + atMod;
        cutoffMod = std::clamp(cutoffMod, 0.0f, 1.0f);

        float cutoffHz = 20.0f * std::pow(10.0f, cutoffMod * 3.0f);
        IR3109FilterDrift::Params dp;
        dp.cutoffHz = cutoffHz;
        dp.resonance = _params.resonance;
        dp.temperature = _params.filterTemp;
        dp.age = _params.filterAge;

        float driftedCutoff = _filterDrift.process(dp);

        float cutoffCV = std::log(std::max(20.0f, driftedCutoff)) / std::log(2.0f) * 0.1f;
        float resCV = std::clamp(_params.resonance, 0.0f, 1.0f);

        float filtered = _filter.process(osc, cutoffCV, resCV, _params.filterTemp);

        float vcaOut = _vca.process(filtered, envVal);

        if (!_env.isActive()) {
            _active = false;
        }

        return vcaOut;
    }

    double age() const { return _age; }

private:
    int _currentNote = -1;
float midiToFreq(int note) {
        return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
    }

    int _index = 0;
    float _sr = 44100.0f;
    bool _active = false;
    double _age = 0.0;

    float _baseFreq = 440.0f;
    float _aftertouch = 0.0f;

    VoiceParams _params;
    IR3109Filter _filter;
    IR3109FilterDrift _filterDrift;
    DCOVoiceDetune _detune;
    AnalogEnvelopeClick _click;
    ExponentialADSR _env;
    DCO _dco;
    JunoLFO _lfo;
    JFETVCA _vca;
};


// ============================================================
