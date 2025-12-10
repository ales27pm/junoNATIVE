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

void JunoDSPEngine::renderAudio(float *L, float *R, int n) {
    if (!running_.load(std::memory_order_acquire) || !L || !R || n <= 0) return;

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
