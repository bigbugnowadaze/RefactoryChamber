#pragma once

#include <juce_dsp/juce_dsp.h>

namespace rc::dsp
{
class InputStage
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        const auto channels = static_cast<int>(spec.numChannels);
        hpFilter.prepare(spec);
        gateEnv.assign(static_cast<size_t>(channels), 0.0f);
        prevDc.assign(static_cast<size_t>(channels), 0.0f);
        dcState.assign(static_cast<size_t>(channels), 0.0f);
        updateCoefficients();
    }

    void reset()
    {
        hpFilter.reset();
        std::fill(gateEnv.begin(), gateEnv.end(), 0.0f);
        std::fill(prevDc.begin(), prevDc.end(), 0.0f);
        std::fill(dcState.begin(), dcState.end(), 0.0f);
    }

    void setParameters(float gateDbIn, float tightIn, float punchIn)
    {
        gateDb = gateDbIn;
        tight = tightIn;
        punch = punchIn;
        updateCoefficients();
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        hpFilter.process(context);

        const float gateThreshold = juce::Decibels::decibelsToGain(gateDb);
        const float attack = std::exp(-1.0f / static_cast<float>(0.001 * sampleRate));
        const float release = std::exp(-1.0f / static_cast<float>(0.035 * sampleRate));
        const float dcA = 0.995f;

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            auto env = gateEnv[static_cast<size_t>(ch)];
            auto prev = prevDc[static_cast<size_t>(ch)];
            auto dc = dcState[static_cast<size_t>(ch)];

            for (int n = 0; n < buffer.getNumSamples(); ++n)
            {
                const float x = data[n];
                dc = dcA * dc + x - prev;
                prev = x;
                float y = dc;

                const float absY = std::abs(y);
                env = absY > env ? attack * env + (1.0f - attack) * absY
                                 : release * env + (1.0f - release) * absY;
                const float gateGain = (env > gateThreshold) ? 1.0f : juce::jmap(env, 0.0f, gateThreshold, 0.05f, 1.0f);
                y *= gateGain;

                const float tilt = 1.0f + 0.65f * punch;
                y = std::tanh(y * tilt);
                data[n] = y;
            }

            gateEnv[static_cast<size_t>(ch)] = env;
            prevDc[static_cast<size_t>(ch)] = prev;
            dcState[static_cast<size_t>(ch)] = dc;
        }
    }

private:
    void updateCoefficients()
    {
        const auto hp = juce::jmap(tight, 0.0f, 1.0f, 25.0f, 120.0f);
        *hpFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, hp);
    }

    double sampleRate{ 48000.0 };
    float gateDb{ -60.0f };
    float tight{ 0.25f };
    float punch{ 0.5f };
    juce::dsp::IIR::Filter<float> hpFilter;
    std::vector<float> gateEnv, prevDc, dcState;
};
}
