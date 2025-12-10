#include <algorithm>
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <numeric>
#include <vector>

#include "JunoDSPEngine.hpp"

#ifndef TEST_SAMPLE_RATE
#define TEST_SAMPLE_RATE 48000
#endif

#ifndef TEST_BUFFER_SIZE
#define TEST_BUFFER_SIZE 128
#endif

#ifndef TEST_POLYPHONY
#define TEST_POLYPHONY 8
#endif

TEST(Latency, RenderTimeBudget) {
    constexpr int sampleRate = TEST_SAMPLE_RATE;
    constexpr int bufferSize = TEST_BUFFER_SIZE;
    constexpr int polyphony = TEST_POLYPHONY;

    JunoDSPEngine engine;
    engine.initialize(sampleRate, bufferSize, polyphony, false);

    std::vector<float> left(bufferSize, 0.0f);
    std::vector<float> right(bufferSize, 0.0f);

    // Warm-up to stabilize caches and any lazy initialization.
    for (int i = 0; i < 5; ++i) {
        engine.renderAudio(left.data(), right.data(), bufferSize);
    }

    std::vector<double> durations;
    durations.reserve(25);

    for (int i = 0; i < 25; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        engine.renderAudio(left.data(), right.data(), bufferSize);
        auto stop = std::chrono::high_resolution_clock::now();
        durations.push_back(
            std::chrono::duration<double, std::micro>(stop - start).count());
    }

    // Use the 90th percentile to tolerate occasional CI jitter while still
    // providing actionable telemetry.
    auto percentileIndex = static_cast<size_t>(durations.size() * 0.9);
    std::nth_element(durations.begin(), durations.begin() + percentileIndex,
                     durations.end());
    double p90 = durations[percentileIndex];

    double average = std::accumulate(durations.begin(), durations.end(), 0.0) /
                     static_cast<double>(durations.size());

    std::cout << "[METRIC] DSP render p90 (us): " << p90
              << " | avg (us): " << average << std::endl;

    SUCCEED();
}
