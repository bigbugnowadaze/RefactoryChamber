#pragma once

#include "Utils.h"
#include "BandSplitter.h"

namespace rc::dsp
{
class ChugField
{
public:
    struct Params
    {
        float ignition{ 0.5f };
        float refractory{ 0.5f };
        float spill{ 0.3f };
        float crush{ 0.5f };
    };

    static constexpr size_t NumBands = BandSplitter::NumBands;

    void prepare(double sr, int controlHopSamplesIn)
    {
        sampleRate = sr;
        controlHopSamples = juce::jmax(8, controlHopSamplesIn);
        reset();
    }

    void reset()
    {
        u.fill(0.0f);
        v.fill(0.0f);
        modulators.fill(1.0f);
        tickCounter = 0;
    }

    void setParameters(const Params& p) { params = p; }

    void processBlock(const std::array<float, NumBands>& envs, int numSamples)
    {
        tickCounter += numSamples;
        while (tickCounter >= controlHopSamples)
        {
            tickCounter -= controlHopSamples;
            tick(envs);
        }
    }

    const std::array<float, 4>& getBandModulators() const noexcept { return modulators; }

    float getAverageExcitation() const
    {
        float sum = 0.0f;
        for (auto x : u) sum += std::max(0.0f, x);
        return sum / static_cast<float>(NumBands);
    }

private:
    void tick(const std::array<float, NumBands>& envs)
    {
        const float dt = static_cast<float>(controlHopSamples / sampleRate);
        const float eps = juce::jmap(params.refractory, 0.0f, 1.0f, 0.9f, 0.1f);
        const float D = 0.05f + params.spill * 0.65f;
        const float kIn = 0.2f + params.ignition * 1.8f;

        std::array<float, NumBands> nu = u;
        std::array<float, NumBands> nv = v;

        for (size_t i = 0; i < NumBands; ++i)
        {
            const float left = i > 0 ? u[i - 1] : u[i];
            const float right = i + 1 < NumBands ? u[i + 1] : u[i];
            const float neighbor = (left + right - 2.0f * u[i]);
            const float input = kIn * envs[i];
            nu[i] = clampSigned(u[i] + dt * (u[i] - (u[i] * u[i] * u[i]) / 3.0f - v[i] + input + D * neighbor));
            nv[i] = clampSigned(v[i] + dt * eps * (u[i] + 0.35f - 0.8f * v[i]));
        }

        u = nu;
        v = nv;

        for (size_t i = 0; i < 4; ++i)
        {
            const float e = std::max(0.0f, u[i * 2]);
            const float r = std::max(0.0f, v[i * 2]);
            modulators[i] = juce::jlimit(0.0f, 2.0f, 1.0f + params.crush * (e - 0.7f * r));
        }
    }

    static float clampSigned(float x) noexcept { return juce::jlimit(-1.5f, 1.5f, x); }

    Params params{};
    double sampleRate{ 48000.0 };
    int controlHopSamples{ 32 };
    int tickCounter{ 0 };
    std::array<float, NumBands> u{}, v{};
    std::array<float, 4> modulators{};
};
}
