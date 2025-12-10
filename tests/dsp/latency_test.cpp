#include <gtest/gtest.h>
#include "JunoDSPEngine.hpp"
#include <chrono>

TEST(Latency, RenderTimeUnder3ms) {
    JunoDSPEngine e;
    e.initialize(48000, 128, 8, false);
    float L[128] = {0.0f};
    float R[128] = {0.0f};

    auto t0 = std::chrono::high_resolution_clock::now();
    e.renderAudio(L, R, 128);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    // Tight but realistic real-time target on a modern CI runner
    EXPECT_LT(us, 3000);
}
