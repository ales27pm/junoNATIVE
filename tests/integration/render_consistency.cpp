#include <gtest/gtest.h>
#include "JunoDSPEngine.hpp"
#include <cmath>

TEST(Render, DeterministicOutput) {
    JunoDSPEngine a;
    JunoDSPEngine b;

    a.initialize(44100, 128, 4, false);
    b.initialize(44100, 128, 4, false);

    float L1[128] = {0.0f};
    float R1[128] = {0.0f};
    float L2[128] = {0.0f};
    float R2[128] = {0.0f};

    a.renderAudio(L1, R1, 128);
    b.renderAudio(L2, R2, 128);

    double diff = 0.0;
    for (int i = 0; i < 128; ++i) {
        diff += std::fabs(L1[i] - L2[i]);
    }

    double meanDiff = diff / 128.0;
    EXPECT_LT(meanDiff, 1e-6);
}
