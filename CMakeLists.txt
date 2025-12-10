cmake_minimum_required(VERSION 3.16)
project(JunoNativeDSP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configurable DSP test parameters (overridable by CI matrix)
set(TEST_SAMPLE_RATE 44100 CACHE STRING "Sample rate used by DSP tests")
set(TEST_BUFFER_SIZE 128 CACHE STRING "Buffer size used by DSP tests")
set(TEST_POLYPHONY 8 CACHE STRING "Polyphony used by DSP tests")

# ------------------------------------------------------------
# GoogleTest via FetchContent (self-contained test setup)
# ------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# For Windows shared CRT handling, keep default.
set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# ------------------------------------------------------------
# DSP engine subdirectory (assumes your engine CMakeLists exists there)
# ------------------------------------------------------------
# If you don't have a nested CMakeLists in rtn-juno-engine/cpp/engine,
# you can instead add the engine sources directly from here.
#
# Example (if you prefer explicit sources):
#   add_library(juno_engine STATIC
#       rtn-juno-engine/cpp/engine/JunoDSPEngine.cpp
#       rtn-juno-engine/cpp/engine/JunoVoice.cpp
#       rtn-juno-engine/cpp/engine/RCUParameterManager.cpp
#   )
#
# For now we keep the structure from the spec:
add_subdirectory(rtn-juno-engine/cpp/engine)

# ------------------------------------------------------------
# Unit tests & benchmarks
# ------------------------------------------------------------
file(GLOB TEST_SRC
  tests/dsp/*.cpp
  tests/integration/*.cpp
)

add_executable(juno_tests ${TEST_SRC})

target_include_directories(juno_tests PRIVATE
  rtn-juno-engine/cpp/engine
  rtn-juno-engine/cpp/dsp
)

target_compile_definitions(juno_tests PRIVATE
  TEST_SAMPLE_RATE=${TEST_SAMPLE_RATE}
  TEST_BUFFER_SIZE=${TEST_BUFFER_SIZE}
  TEST_POLYPHONY=${TEST_POLYPHONY}
)

target_link_libraries(juno_tests PRIVATE
  gtest_main
  juno_engine               # Provided by rtn-juno-engine/cpp/engine CMake
)

add_test(NAME juno_tests COMMAND juno_tests)
