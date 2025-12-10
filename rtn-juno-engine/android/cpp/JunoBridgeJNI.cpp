#include <jni.h>
#include <memory>
#include "JunoAudioEngine.hpp"

static std::unique_ptr<JunoAudioEngine> engine;

extern "C" {

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeStart(JNIEnv * /*env*/,
                                                       jobject /*thiz*/,
                                                       jint sr,
                                                       jint bs) {
    if (!engine) {
        engine = std::make_unique<JunoAudioEngine>();
    }
    engine->start(static_cast<int>(sr), static_cast<int>(bs));
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeStop(JNIEnv * /*env*/,
                                                      jobject /*thiz*/) {
    if (engine) {
        engine->stop();
    }
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeNoteOn(JNIEnv * /*env*/,
                                                        jobject /*thiz*/,
                                                        jint note,
                                                        jfloat vel) {
    if (engine) {
        engine->noteOn(static_cast<int>(note), static_cast<float>(vel));
    }
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeNoteOff(JNIEnv * /*env*/,
                                                         jobject /*thiz*/,
                                                         jint note) {
    if (engine) {
        engine->noteOff(static_cast<int>(note));
    }
}

JNIEXPORT void JNICALL
Java_com_pulsr_junonative_JunoEngineModule_nativeSetParam(JNIEnv *env,
                                                          jobject /*thiz*/,
                                                          jstring id,
                                                          jfloat val) {
    const char *cid = env->GetStringUTFChars(id, nullptr);
    if (engine && cid) {
        engine->setParameter(cid, static_cast<float>(val));
    }
    if (cid) {
        env->ReleaseStringUTFChars(id, cid);
    }
}

} // extern "C"
