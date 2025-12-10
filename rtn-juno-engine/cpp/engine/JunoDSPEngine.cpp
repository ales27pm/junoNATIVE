#include "JunoDSPEngine.hpp"
#include "../ios/JunoRenderEngine.hpp"
#include <algorithm>
#include <cmath>

namespace Juno106 {
    struct JunoPatch {
        // Minimal subset needed by this engine. If you already have a richer
        // JunoPatch in your project, remove this and include the real header.
        unsigned char vcfCutoff   = 100;
        unsigned char vcfResonance= 20;
        unsigned char envAttack   = 20;
        unsigned char envRelease  = 40;
        unsigned char dcoSubLevel = 0;
    };
}

bool JunoDSPEngine::initialize(int sr, int bs, int poly, bool gpuFlag) {
    sampleRate_ = sr;
    bufferSize_ = bs;
    useGPU_     = gpuFlag;

    voices_.clear();
    voices_.reserve(poly);
    for (int i = 0; i < poly; ++i) {
        auto v = std::make_unique<JunoVoice>();
        v->initialize(sampleRate_);
        voices_.push_back(std::move(v));
    }

    if (useGPU_) {
        gpu_ = std::make_unique<JunoRenderEngine>();
        if (!gpu_->initialize(static_cast<float>(sampleRate_), poly, bufferSize_)) {
            gpu_.reset();
            useGPU_ = false;
            gpuVoiceCache_.reset();
        } else {
            gpuVoiceCache_ = std::make_unique<std::vector<VoiceGPUParams>>(voices_.size());
        }
    } else {
        gpu_.reset();
        gpuVoiceCache_.reset();
    }

    running_.store(true, std::memory_order_release);
    return true;
}

void JunoDSPEngine::start()  { running_.store(true, std::memory_order_release); }
void JunoDSPEngine::stop()   { running_.store(false, std::memory_order_release); }

void JunoDSPEngine::noteOn(int note, float vel) {
    if (voices_.empty()) {
        return;
    }

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
    auto map01 = [](unsigned char v) {
        return static_cast<float>(v) / 127.0f;
    };

    float cutoffNorm    = map01(p.vcfCutoff);
    float cutoffHz      = std::exp(std::log(50.0f) +
                             (std::log(15000.0f) - std::log(50.0f)) * cutoffNorm);

    float resonanceNorm = map01(p.vcfResonance);
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
    if (!running_.load(std::memory_order_acquire) || !L || !R || n <= 0) return;

    // Apply any pending parameter changes on the audio thread to avoid races.
    RCUParameterManager::ParamChange change;
    while (params_.tryPop(change)) {
        for (auto &voice : voices_) {
            voice->setParam(change.id, change.value);
        }
    }

    if (useGPU_ && gpu_) {
        // Collect voice state for GPU without heap allocations on the audio thread.
        if (gpuVoiceCache_ && gpuVoiceCache_->size() >= voices_.size()) {
            for (std::size_t i = 0; i < voices_.size(); ++i) {
                const auto &v = voices_[i];
                VoiceGPUParams &vp = (*gpuVoiceCache_)[i];
                vp.frequency  = v->frequency_;
                vp.velocity   = v->velocity_;
                vp.envelope   = v->envelopeLevel();
                vp.phase[0]   = v->phase();
                vp.phase[1]   = 0.0f;
                vp.phase[2]   = 0.0f;
                vp.pulseWidth = v->pulseWidth();
                vp.active     = v->isActive() ? 1u : 0u;
            }
            gpu_->updateVoices(*gpuVoiceCache_);
        }
        gpu_->render(L, R, n);
        for (auto &voice : voices_) {
            voice->advanceState(n);
        }
        return;
    }

    // CPU path
    std::fill(L, L + n, 0.0f);
    std::fill(R, R + n, 0.0f);

    for (auto &voice : voices_) {
        for (int i = 0; i < n; ++i) {
            float l = 0.0f;
            float r = 0.0f;
            voice->process(l, r);
            L[i] += l;
            R[i] += r;
        }
    }
}
