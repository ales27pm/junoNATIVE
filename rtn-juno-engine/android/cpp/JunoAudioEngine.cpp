#include "JunoAudioEngine.hpp"

JunoAudioEngine::JunoAudioEngine() {
    dsp_ = std::make_unique<JunoDSPEngine>();
}

JunoAudioEngine::~JunoAudioEngine() {
    stop();
}

bool JunoAudioEngine::start(int sr, int bs) {
    std::lock_guard<std::mutex> lock(streamMutex_);

    if (stream_) {
        return true;
    }

    if (!dsp_) {
        dsp_ = std::make_unique<JunoDSPEngine>();
    }

    // Android path uses CPU DSP only (no GPU).
    if (!dsp_->initialize(sr, bs, 8, false)) {
        return false;
    }

    AAudioStreamBuilder *builder = nullptr;
    aaudio_result_t res = AAudio_createStreamBuilder(&builder);
    if (res != AAUDIO_OK || !builder) {
        return false;
    }

    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setSampleRate(builder, sr);
    AAudioStreamBuilder_setChannelCount(builder, 2);
    AAudioStreamBuilder_setFramesPerDataCallback(builder, bs);
    AAudioStreamBuilder_setDataCallback(builder, renderCB, this);

    ensureBuffers(static_cast<size_t>(bs));

    res = AAudioStreamBuilder_openStream(builder, &stream_);
    AAudioStreamBuilder_delete(builder);

    if (res != AAUDIO_OK || !stream_) {
        stream_ = nullptr;
        return false;
    }

    res = AAudioStream_requestStart(stream_);
    if (res != AAUDIO_OK) {
        AAudioStream_close(stream_);
        stream_ = nullptr;
        return false;
    }

    return true;
}

void JunoAudioEngine::stop() {
    std::lock_guard<std::mutex> lock(streamMutex_);

    if (stream_) {
        AAudioStream_requestStop(stream_);
        AAudioStream_close(stream_);
        stream_ = nullptr;
    }
}

aaudio_data_callback_result_t
JunoAudioEngine::renderCB(AAudioStream * /*stream*/,
                          void *user,
                          void *audioData,
                          int32_t frames) {
    auto *self = reinterpret_cast<JunoAudioEngine *>(user);
    if (!self || !self->dsp_) {
        return AAUDIO_CALLBACK_RESULT_CONTINUE;
    }

    float *buffer = static_cast<float *>(audioData);
    self->ensureBuffers(static_cast<size_t>(frames));

    self->dsp_->renderAudio(self->leftBuffer_.data(),
                            self->rightBuffer_.data(),
                            frames);

    for (int i = 0; i < frames; ++i) {
        buffer[i * 2]     = self->leftBuffer_[i];
        buffer[i * 2 + 1] = self->rightBuffer_[i];
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void JunoAudioEngine::ensureBuffers(size_t frames) {
    if (leftBuffer_.size() < frames) {
        leftBuffer_.resize(frames);
    }
    if (rightBuffer_.size() < frames) {
        rightBuffer_.resize(frames);
    }
}

void JunoAudioEngine::noteOn(int n, float v) {
    if (dsp_) {
        dsp_->noteOn(n, v);
    }
}

void JunoAudioEngine::noteOff(int n) {
    if (dsp_) {
        dsp_->noteOff(n);
    }
}

void JunoAudioEngine::setParameter(const std::string &id, float v) {
    if (dsp_) {
        dsp_->setParameter(id, v);
    }
}

void JunoAudioEngine::loadPatch(const Juno106::JunoPatch &p) {
    if (dsp_) {
        dsp_->loadPatch(p);
    }
}
