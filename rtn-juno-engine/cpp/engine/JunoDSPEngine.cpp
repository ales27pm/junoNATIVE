#include "JunoDSPEngine.hpp"
#include <algorithm>

bool JunoDSPEngine::initialize(int sr, int bs, int poly, bool gpu) {
    sampleRate_ = sr;
    bufferSize_ = bs;
    useGPU_     = gpu;

    voices_.clear();
    voices_.reserve(poly);
    for (int i = 0; i < poly; ++i) {
        auto v = std::make_unique<JunoVoice>();
        v->initialize(sampleRate_);
        voices_.push_back(std::move(v));
    }

    params_.clear();
    running_ = true;
    return true;
}

void JunoDSPEngine::start()  { running_ = true; }
void JunoDSPEngine::stop()   { running_ = false; }

void JunoDSPEngine::noteOn(int note, float vel) {
    // Basic voice allocation: first free voice, else steal voice 0
    auto it = std::find_if(
        voices_.begin(),
        voices_.end(),
        [](const std::unique_ptr<JunoVoice> &v) { return !v->isActive(); }
    );
    if (it == voices_.end()) {
        it = voices_.begin();
    }
    (*it)->noteOn(note, vel);
}

void JunoDSPEngine::noteOff(int note) {
    for (auto &v : voices_) {
        v->noteOff(note);
    }
}

void JunoDSPEngine::setParameter(const std::string &id, float v) {
    params_.set(id, v);
}

void JunoDSPEngine::loadPatch(const Juno106::JunoPatch &p) {
    // Map raw 0–127 patch values to musically sensible ranges.
    auto map01 = [](unsigned char v) {
        return static_cast<float>(v) / 127.0f;
    };

    float cutoffNorm    = map01(p.vcfCutoff);
    // 50 Hz → 15 kHz log curve
    float cutoffHz      = std::exp(std::log(50.0f) +
                             (std::log(15000.0f) - std::log(50.0f)) * cutoffNorm);

    float resonanceNorm = map01(p.vcfResonance); // 0..1
    float attackTime    = 0.0015f * std::pow(10.0f, map01(p.envAttack) * 3.0f);
    float releaseTime   = 0.0015f * std::pow(10.0f, map01(p.envRelease) * 3.6f);
    float subLevel      = map01(p.dcoSubLevel);

    setParameter("cutoff",    cutoffHz);
    setParameter("resonance", resonanceNorm);
    setParameter("attack",    attackTime);
    setParameter("release",   releaseTime);
    setParameter("subLevel",  subLevel);
}

void JunoDSPEngine::renderAudio(float *L, float *R, int n) {
    if (!running_ || !L || !R || n <= 0) return;

    std::fill(L, L + n, 0.0f);
    std::fill(R, R + n, 0.0f);

    RCUParameterManager::ParamChange change;
    while (params_.tryPop(change)) {
        for (auto &voice : voices_) {
            voice->setParam(change.id, change.value);
        }
    }

    for (auto &voice : voices_) {
        voice->processBlock(L, R, n);
    }
}
