#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace rc::state
{
struct Params
{
    static constexpr auto gate = "gate";
    static constexpr auto tight = "tight";
    static constexpr auto punch = "punch";

    static constexpr auto heat = "heat";
    static constexpr auto imprint = "imprint";
    static constexpr auto hardening = "hardening";
    static constexpr auto relax = "relax";

    static constexpr auto ignition = "ignition";
    static constexpr auto refractory = "refractory";
    static constexpr auto spill = "spill";
    static constexpr auto crush = "crush";

    static constexpr auto core = "core";
    static constexpr auto defect = "defect";
    static constexpr auto gap = "gap";
    static constexpr auto bite = "bite";

    static constexpr auto collapse = "collapse";
    static constexpr auto mix = "mix";
    static constexpr auto level = "level";
    static constexpr auto oversampling = "oversampling";
};

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using P = juce::AudioParameterFloat;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    auto norm = juce::NormalisableRange<float>(0.0f, 1.0f);

    p.push_back(std::make_unique<P>(Params::gate, "Gate", juce::NormalisableRange<float>(-80.0f, -20.0f), -60.0f));
    p.push_back(std::make_unique<P>(Params::tight, "Tight", norm, 0.35f));
    p.push_back(std::make_unique<P>(Params::punch, "Punch", norm, 0.5f));

    p.push_back(std::make_unique<P>(Params::heat, "Heat", norm, 0.55f));
    p.push_back(std::make_unique<P>(Params::imprint, "Imprint", norm, 0.5f));
    p.push_back(std::make_unique<P>(Params::hardening, "Hardening", norm, 0.6f));
    p.push_back(std::make_unique<P>(Params::relax, "Relax", norm, 0.4f));

    p.push_back(std::make_unique<P>(Params::ignition, "Ignition", norm, 0.55f));
    p.push_back(std::make_unique<P>(Params::refractory, "Refractory", norm, 0.5f));
    p.push_back(std::make_unique<P>(Params::spill, "Spill", norm, 0.35f));
    p.push_back(std::make_unique<P>(Params::crush, "Crush", norm, 0.5f));

    p.push_back(std::make_unique<P>(Params::core, "Core", norm, 0.5f));
    p.push_back(std::make_unique<P>(Params::defect, "Defect", norm, 0.45f));
    p.push_back(std::make_unique<P>(Params::gap, "Gap", norm, 0.5f));
    p.push_back(std::make_unique<P>(Params::bite, "Bite", norm, 0.55f));

    p.push_back(std::make_unique<P>(Params::collapse, "Collapse", norm, 0.35f));
    p.push_back(std::make_unique<P>(Params::mix, "Mix", norm, 1.0f));
    p.push_back(std::make_unique<P>(Params::level, "Level", juce::NormalisableRange<float>(-24.0f, 12.0f), -2.0f));

    p.push_back(std::make_unique<juce::AudioParameterChoice>(Params::oversampling, "Oversampling",
        juce::StringArray{ "Off", "2x", "4x" }, 1));

    return { p.begin(), p.end() };
}
}
