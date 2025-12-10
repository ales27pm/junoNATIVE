#pragma once
#include <aaudio/AAudio.h>
#include <memory>
#include <string>
#include "JunoDSPEngine.hpp"

class JunoAudioEngine {
public:
    JunoAudioEngine();
    bool start(int sampleRate, int bufferSize);
    void stop();

    void noteOn(int note, float vel);
    void noteOff(int note);
    void setParameter(const std::string &id, float value);
    void loadPatch(const Juno106::JunoPatch &patch);

private:
    static aaudio_data_callback_result_t renderCB(AAudioStream *stream,
                                                  void *userData,
                                                  void *audioData,
                                                  int32_t numFrames);

    AAudioStream *stream_ = nullptr;
    std::unique_ptr<JunoDSPEngine> dsp_;
};
