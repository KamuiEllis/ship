#include "SoundSynth.h"
#include <cmath>

constexpr float PI = 3.14159265358979323846f;

std::vector<float> generateBeep(
    float frequency,
    float duration
)
{
    int sampleCount = static_cast<int>(
        SAMPLE_RATE * duration
        );

    std::vector<float> samples(sampleCount);

    for (int i = 0; i < sampleCount; i++)
    {
        float time = static_cast<float>(i) / SAMPLE_RATE;

        float wave = std::sin(
            2.0f * PI * frequency * time
        );

        // Reduce volume and fade out
        float envelope = 1.0f - time / duration;

        samples[i] = wave * envelope * 0.2f;
    }

    return samples;
}