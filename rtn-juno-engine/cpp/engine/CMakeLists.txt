cmake_minimum_required(VERSION 3.16)

set(JUNO_ENGINE_SOURCES
    JunoDSPEngine.cpp
    JunoVoice.cpp
    RCUParameterManager.cpp
)

add_library(juno_engine STATIC ${JUNO_ENGINE_SOURCES})

target_compile_features(juno_engine PUBLIC cxx_std_17)

target_include_directories(juno_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../dsp
        ${PROJECT_SOURCE_DIR}/cpp/parser
        ${PROJECT_SOURCE_DIR}/cpp/dsp
        ${PROJECT_SOURCE_DIR}/rtn-juno-engine/ios
)
