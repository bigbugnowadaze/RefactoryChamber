#pragma once

#include <array>
#include <juce_dsp/juce_dsp.h>

namespace rc::dsp
{
class Oversampler
{
public:
    void prepare(size_t channelsIn, size_t maxBlockSizeIn)
    {
        channels = channelsIn;
        maxSamples = maxBlockSizeIn;

        oversamplers[0].reset();
        oversamplers[1] = createOversampler(1u);
        oversamplers[2] = createOversampler(2u);
        setMode(currentMode);
    }

    void reset()
    {
        for (auto& os : oversamplers)
            if (os != nullptr)
                os->reset();
    }

    void setMode(int mode)
    {
        currentMode = juce::jlimit(0, 2, mode);
        active = oversamplers[static_cast<size_t>(currentMode)].get();
    }

    int getMode() const noexcept { return currentMode; }
    int getLatencySamples() const { return active != nullptr ? static_cast<int>(active->getLatencyInSamples()) : 0; }

    template <typename Fn>
    void process(juce::AudioBuffer<float>& buffer, Fn&& fn)
    {
        if (currentMode == 0 || active == nullptr)
        {
            fn(juce::dsp::AudioBlock<float>(buffer));
            return;
        }

        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto upBlock = active->processSamplesUp(block);
        fn(upBlock);
        active->processSamplesDown(block);
    }

private:
    std::unique_ptr<juce::dsp::Oversampling<float>> createOversampler(size_t factor)
    {
        auto os = std::make_unique<juce::dsp::Oversampling<float>>(channels, factor,
            juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);
        os->reset();
        os->initProcessing(maxSamples);
        return os;
    }

    int currentMode{ 1 }; // 0:off,1:2x,2:4x
    size_t channels{ 2 }, maxSamples{ 512 };
    std::array<std::unique_ptr<juce::dsp::Oversampling<float>>, 3> oversamplers;
    juce::dsp::Oversampling<float>* active{ nullptr };
};
}
