#include <gtest/gtest.h>
#include "JunoDSPEngine.hpp"
#include <chrono>

TEST(CPU, AverageRenderSpeed) {
    JunoDSPEngine e;
    e.initialize(44100, 256, 16, false);
    float L[256] = {0.0f};
    float R[256] = {0.0f};

    double total = 0.0;
    const int iterations = 200;

    for (int i = 0; i < iterations; ++i) {
        auto s = std::chrono::steady_clock::now();
        e.renderAudio(L, R, 256);
        auto d = std::chrono::steady_clock::now() - s;
        total += std::chrono::duration<double, std::micro>(d).count();
    }

    double avg = total / static_cast<double>(iterations);
    // Average per-block render time (in microseconds) under 5ms.
    EXPECT_LT(avg, 5000.0);
}
