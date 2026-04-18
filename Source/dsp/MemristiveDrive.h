#pragma once

#include "Utils.h"
#include <juce_dsp/juce_dsp.h>

namespace rc::dsp
{
class MemristiveDrive
{
public:
    struct Params
    {
        float heat{ 0.5f };
        float imprint{ 0.5f };
        float hardening{ 0.5f };
        float relax{ 0.5f };
    };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        reset();
    }

    void reset()
    {
        env = 0.0f;
        memory = 0.0f;
        prevSample = 0.0f;
        lastEffDrive = 1.0f;
    }

    void setParameters(const Params& p) { params = p; }

    float processSample(float x) noexcept
    {
        const float absX = std::abs(x);
        const float envA = std::exp(-1.0f / static_cast<float>(0.01 * sampleRate));
        env = juce::jmax(absX, envA * env + (1.0f - envA) * absX);
        const float slope = std::abs(x - prevSample);
        prevSample = x;

        const float relaxCoeff = juce::jmap(params.relax, 0.0f, 1.0f, 0.004f, 0.1f);
        const float i1 = 0.08f + 0.75f * params.imprint;
        const float i2 = 0.04f + 0.95f * params.hardening;
        memory = clamp01((1.0f - relaxCoeff) * memory + i1 * env * 0.1f + i2 * slope * 0.1f);

        const float baseDrive = juce::jmap(params.heat, 0.0f, 1.0f, 1.0f, 16.0f);
        const float gEff = baseDrive + (1.2f + 6.0f * params.hardening) * memory;
        lastEffDrive = gEff;
        const float bEff = (memory - 0.5f) * (0.55f * params.imprint);

        return fastSat(gEff * x + bEff, 0.1f + 0.35f * params.hardening);
    }

    template <typename Block>
    void processBlock(Block& block) noexcept
    {
        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
            for (size_t n = 0; n < block.getNumSamples(); ++n)
                block.getChannelPointer(ch)[n] = processSample(block.getChannelPointer(ch)[n]);
    }

    float getMemoryState() const noexcept { return memory; }
    float getEffectiveDrive() const noexcept { return lastEffDrive; }

private:
    Params params{};
    double sampleRate{ 48000.0 };
    float env{}, memory{}, prevSample{}, lastEffDrive{};
};
}
