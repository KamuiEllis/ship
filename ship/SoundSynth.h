#pragma once
#include <vector>

constexpr int SAMPLE_RATE = 48000;

std::vector<float> generateBeep(
    float frequency,
    float duration
);