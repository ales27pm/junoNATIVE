#include <algorithm>
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <numeric>
#include <vector>

#include "JunoDSPEngine.hpp"

#ifndef TEST_SAMPLE_RATE
#define TEST_SAMPLE_RATE 44100
#endif

#ifndef TEST_BUFFER_SIZE
#define TEST_BUFFER_SIZE 256
#endif

#ifndef TEST_POLYPHONY
#define TEST_POLYPHONY 16
#endif

TEST(CPU, AverageRenderSpeed) {
    constexpr int sampleRate = TEST_SAMPLE_RATE;
    constexpr int bufferSize = TEST_BUFFER_SIZE;
    constexpr int polyphony = TEST_POLYPHONY;

    JunoDSPEngine engine;
    engine.initialize(sampleRate, bufferSize, polyphony, false);

    std::vector<float> left(bufferSize, 0.0f);
    std::vector<float> right(bufferSize, 0.0f);

    // Warm-up to mitigate cache and allocation noise.
    for (int i = 0; i < 10; ++i) {
        engine.renderAudio(left.data(), right.data(), bufferSize);
    }

    std::vector<double> durations;
    durations.reserve(100);

    for (int i = 0; i < 100; ++i) {
        auto start = std::chrono::steady_clock::now();
        engine.renderAudio(left.data(), right.data(), bufferSize);
        auto end = std::chrono::steady_clock::now();
        durations.push_back(
            std::chrono::duration<double, std::micro>(end - start).count());
    }

    double averageUs = std::accumulate(durations.begin(), durations.end(), 0.0) /
                       static_cast<double>(durations.size());

    auto percentileIndex = static_cast<size_t>(durations.size() * 0.9);
    std::nth_element(durations.begin(), durations.begin() + percentileIndex,
                     durations.end());
    double p90 = durations[percentileIndex];

    std::cout << "[METRIC] DSP CPU avg (us): " << averageUs
              << " | p90 (us): " << p90 << std::endl;

    SUCCEED();
}
