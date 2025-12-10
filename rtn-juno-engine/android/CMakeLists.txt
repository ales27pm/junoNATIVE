cmake_minimum_required(VERSION 3.10.2)
project(junobridge)

# Build JNI + audio bridge and link to the shared DSP core.
add_library(junobridge SHARED
    cpp/JunoAudioEngine.cpp
    cpp/JunoBridgeJNI.cpp
    ../../cpp/engine/JunoDSPEngine.cpp
    ../../cpp/engine/JunoVoice.cpp
    ../../cpp/engine/RCUParameterManager.cpp
)

# Include paths so headers like "JunoDSPEngine.hpp" resolve cleanly.
target_include_directories(junobridge PRIVATE
    ../../cpp/engine
    ../../cpp/dsp
)

find_library(log-lib log)
find_library(aaudio-lib aaudio)

target_link_libraries(junobridge
    ${log-lib}
    ${aaudio-lib}
)
