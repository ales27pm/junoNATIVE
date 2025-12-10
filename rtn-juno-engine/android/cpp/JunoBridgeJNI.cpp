#include <jni.h>
#include <memory>
#include <mutex>
#include "JunoAudioEngine.hpp"

static std::shared_ptr<JunoAudioEngine> engine;
static std::mutex engineMutex;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeStart(JNIEnv * /*env*/,
                                                       jobject /*thiz*/,
                                                       jint sr,
                                                       jint bs) {
    std::shared_ptr<JunoAudioEngine> localEngine;
    {
        std::lock_guard<std::mutex> lock(engineMutex);
        if (!engine) {
            engine = std::make_shared<JunoAudioEngine>();
        }
        localEngine = engine;
    }

    if (!localEngine) {
        return JNI_FALSE;
    }

    const bool started = localEngine->start(static_cast<int>(sr), static_cast<int>(bs));
    return started ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeStop(JNIEnv * /*env*/,
                                                      jobject /*thiz*/) {
    std::lock_guard<std::mutex> lock(engineMutex);
    if (engine) {
        engine->stop();
        engine.reset();
    }
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeNoteOn(JNIEnv * /*env*/,
                                                        jobject /*thiz*/,
                                                        jint note,
                                                        jfloat vel) {
    std::shared_ptr<JunoAudioEngine> localEngine;
    {
        std::lock_guard<std::mutex> lock(engineMutex);
        localEngine = engine;
    }

    if (localEngine) {
        localEngine->noteOn(static_cast<int>(note), static_cast<float>(vel));
    }
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeNoteOff(JNIEnv * /*env*/,
                                                         jobject /*thiz*/,
                                                         jint note) {
    std::shared_ptr<JunoAudioEngine> localEngine;
    {
        std::lock_guard<std::mutex> lock(engineMutex);
        localEngine = engine;
    }

    if (localEngine) {
        localEngine->noteOff(static_cast<int>(note));
    }
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeSetParam(JNIEnv *env,
                                                          jobject /*thiz*/,
                                                          jstring id,
                                                          jfloat val) {
    if (id == nullptr) {
        return;
    }

    const char *cid = env->GetStringUTFChars(id, nullptr);
    std::shared_ptr<JunoAudioEngine> localEngine;
    {
        std::lock_guard<std::mutex> lock(engineMutex);
        localEngine = engine;
    }

    if (localEngine && cid) {
        localEngine->setParameter(cid, static_cast<float>(val));
    }
    if (cid) {
        env->ReleaseStringUTFChars(id, cid);
    }
}

} // extern "C"
