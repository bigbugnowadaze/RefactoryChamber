#pragma once

#include <array>
#include <juce_dsp/juce_dsp.h>

namespace rc::dsp
{
class BandSplitter
{
public:
    static constexpr size_t NumBands = 8;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        for (auto& f : filters)
            f.prepare(spec);
        reset();
        updateFilters();
    }

    void reset()
    {
        for (auto& f : filters) f.reset();
        envelopes.fill(0.0f);
    }

    void process(const juce::AudioBuffer<float>& monoBuffer)
    {
        constexpr std::array<float, NumBands> centers{ 80.f, 130.f, 220.f, 380.f, 700.f, 1300.f, 2400.f, 4200.f };
        const float envCoeff = std::exp(-1.0f / static_cast<float>(0.008 * sampleRate));

        for (size_t b = 0; b < NumBands; ++b)
        {
            auto env = envelopes[b];
            auto& f = filters[b];
            for (int n = 0; n < monoBuffer.getNumSamples(); ++n)
            {
                const float x = monoBuffer.getSample(0, n);
                const float y = f.processSample(x);
                const float a = std::abs(y);
                env = juce::jmax(a, envCoeff * env + (1.0f - envCoeff) * a);
            }
            envelopes[b] = juce::jlimit(0.0f, 2.0f, env);
        }
    }

    const std::array<float, NumBands>& getEnvelopes() const noexcept { return envelopes; }

private:
    void updateFilters()
    {
        constexpr std::array<float, NumBands> centers{ 80.f, 130.f, 220.f, 380.f, 700.f, 1300.f, 2400.f, 4200.f };
        for (size_t i = 0; i < NumBands; ++i)
            *filters[i].state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, centers[i], 0.707f);
    }

    double sampleRate{ 48000.0 };
    std::array<juce::dsp::IIR::Filter<float>, NumBands> filters;
    std::array<float, NumBands> envelopes{};
};
}
