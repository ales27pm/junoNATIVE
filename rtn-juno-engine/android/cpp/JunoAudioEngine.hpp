#pragma once
#include <aaudio/AAudio.h>
#include <memory>
#include <string>
#include <mutex>
#include <vector>
#include "JunoDSPEngine.hpp"

class JunoAudioEngine {
public:
    JunoAudioEngine();
    ~JunoAudioEngine();

    JunoAudioEngine(const JunoAudioEngine &) = delete;
    JunoAudioEngine &operator=(const JunoAudioEngine &) = delete;
    JunoAudioEngine(JunoAudioEngine &&) = delete;
    JunoAudioEngine &operator=(JunoAudioEngine &&) = delete;

    bool start(int sampleRate, int bufferSize);
    void stop();

    void noteOn(int note, float vel);
    void noteOff(int note);
    void setParameter(const std::string &id, float value);
    void loadPatch(const Juno106::JunoPatch &patch);

private:
    void ensureBuffers(size_t frames);
    static aaudio_data_callback_result_t renderCB(AAudioStream *stream,
                                                  void *userData,
                                                  void *audioData,
                                                  int32_t numFrames);

    std::mutex streamMutex_;
    AAudioStream *stream_ = nullptr;
    std::unique_ptr<JunoDSPEngine> dsp_;
    std::vector<float> leftBuffer_;
    std::vector<float> rightBuffer_;
};
