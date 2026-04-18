#pragma once

#include <juce_core/juce_core.h>

namespace rc::dsp
{
class CollapseState
{
public:
    enum class Mode { normal, armed, collapsed };

    void prepare(double sr)
    {
        sampleRate = sr;
        reset();
    }

    void reset()
    {
        stress = 0.0f;
        mode = Mode::normal;
    }

    void setAmount(float v) { collapse = juce::jlimit(0.0f, 1.0f, v); }

    void update(float memory, float field, float env)
    {
        const float target = juce::jlimit(0.0f, 1.5f, 0.45f * memory + 0.35f * field + 0.2f * env);
        const float a = std::exp(-1.0f / static_cast<float>(0.02 * sampleRate));
        stress = a * stress + (1.0f - a) * target;

        const float armT = juce::jmap(collapse, 0.0f, 1.0f, 0.85f, 0.55f);
        const float collapseT = armT + 0.12f;
        const float releaseT = armT - 0.15f;

        switch (mode)
        {
            case Mode::normal: if (stress > armT) mode = Mode::armed; break;
            case Mode::armed:
                if (stress > collapseT) mode = Mode::collapsed;
                else if (stress < releaseT) mode = Mode::normal;
                break;
            case Mode::collapsed:
                if (stress < armT) mode = Mode::armed;
                break;
        }
    }

    float getDriveBoost() const
    {
        return mode == Mode::collapsed ? 1.25f + 0.75f * collapse
             : mode == Mode::armed ? 1.1f : 1.0f;
    }

    float getStress() const noexcept { return stress; }
    Mode getMode() const noexcept { return mode; }

private:
    double sampleRate{ 48000.0 };
    float collapse{ 0.5f };
    float stress{ 0.0f };
    Mode mode{ Mode::normal };
};
}
