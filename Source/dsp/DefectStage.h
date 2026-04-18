#pragma once

#include <array>
#include <juce_dsp/juce_dsp.h>

namespace rc::dsp
{
class DefectStage
{
public:
    struct Params
    {
        float core{ 0.5f };
        float defect{ 0.5f };
        float gap{ 0.5f };
        float bite{ 0.5f };
    };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        for (auto& f : corridors) f.prepare(spec);
        mudNotch.prepare(spec);
        coreShelf.prepare(spec);
        reset();
        updateCoefficients();
    }

    void reset()
    {
        for (auto& f : corridors) f.reset();
        mudNotch.reset();
        coreShelf.reset();
    }

    void setParameters(const Params& p)
    {
        params = p;
        updateCoefficients();
    }

    float processSample(float x, float memState, float fieldExcitation)
    {
        float y = coreShelf.processSample(x);
        y = mudNotch.processSample(y);

        const float dyn = 0.5f * memState + 0.5f * fieldExcitation;
        float add = 0.0f;
        for (size_t i = 0; i < corridors.size(); ++i)
        {
            const float c = corridors[i].processSample(y);
            const float g = corridorGains[i] * (1.0f + dyn * (0.15f + 0.2f * params.bite));
            add += c * g;
        }

        return std::tanh(y + add);
    }

private:
    void updateCoefficients()
    {
        const float coreGain = juce::jmap(params.core, 0.0f, 1.0f, -3.0f, 5.0f);
        *coreShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 180.0f, 0.707f, juce::Decibels::decibelsToGain(coreGain));

        const float gapCenter = juce::jmap(params.defect, 0.0f, 1.0f, 260.0f, 520.0f);
        const float q = juce::jmap(params.gap, 0.0f, 1.0f, 0.5f, 1.8f);
        *mudNotch.state = *juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, gapCenter, q);

        constexpr std::array<float, 3> freqs{ 850.0f, 1700.0f, 3300.0f };
        for (size_t i = 0; i < corridors.size(); ++i)
            *corridors[i].state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, freqs[i], 0.85f);

        corridorGains = {
            0.2f + 0.6f * params.defect,
            0.3f + 0.7f * params.bite,
            0.2f + 0.9f * params.bite
        };
    }

    double sampleRate{ 48000.0 };
    Params params{};
    juce::dsp::IIR::Filter<float> coreShelf;
    juce::dsp::IIR::Filter<float> mudNotch;
    std::array<juce::dsp::IIR::Filter<float>, 3> corridors;
    std::array<float, 3> corridorGains{};
};
}
