#include <cmath>
#include <gtest/gtest.h>
#include <vector>

#include "JunoDSPEngine.hpp"

#ifndef TEST_SAMPLE_RATE
#define TEST_SAMPLE_RATE 44100
#endif

#ifndef TEST_BUFFER_SIZE
#define TEST_BUFFER_SIZE 128
#endif

#ifndef TEST_POLYPHONY
#define TEST_POLYPHONY 4
#endif

TEST(Render, DeterministicOutput) {
    constexpr int sampleRate = TEST_SAMPLE_RATE;
    constexpr int bufferSize = TEST_BUFFER_SIZE;
    constexpr int polyphony = TEST_POLYPHONY;
    constexpr int blocks = 3;

    JunoDSPEngine a;
    JunoDSPEngine b;

    a.initialize(sampleRate, bufferSize, polyphony, false);
    b.initialize(sampleRate, bufferSize, polyphony, false);

    std::vector<float> leftA(bufferSize, 0.0f);
    std::vector<float> rightA(bufferSize, 0.0f);
    std::vector<float> leftB(bufferSize, 0.0f);
    std::vector<float> rightB(bufferSize, 0.0f);

    double leftDiff = 0.0;
    double rightDiff = 0.0;

    for (int block = 0; block < blocks; ++block) {
        std::fill(leftA.begin(), leftA.end(), 0.0f);
        std::fill(rightA.begin(), rightA.end(), 0.0f);
        std::fill(leftB.begin(), leftB.end(), 0.0f);
        std::fill(rightB.begin(), rightB.end(), 0.0f);

        a.renderAudio(leftA.data(), rightA.data(), bufferSize);
        b.renderAudio(leftB.data(), rightB.data(), bufferSize);

        for (int i = 0; i < bufferSize; ++i) {
            leftDiff += std::fabs(leftA[i] - leftB[i]);
            rightDiff += std::fabs(rightA[i] - rightB[i]);
        }
    }

    const double denom = static_cast<double>(bufferSize * blocks);
    double meanLeftDiff = leftDiff / denom;
    double meanRightDiff = rightDiff / denom;

    EXPECT_LT(meanLeftDiff, 1e-6);
    EXPECT_LT(meanRightDiff, 1e-6);
}
