#pragma once

#include "ParameterLayout.h"
#include <map>

namespace rc::state
{
struct FactoryPreset
{
    juce::String name;
    std::map<juce::String, float> values;
};

inline std::vector<FactoryPreset> getFactoryPresets()
{
    return {
        { "Surgical Chug", {
            { Params::tight, 0.72f }, { Params::heat, 0.62f }, { Params::hardening, 0.75f }, { Params::refractory, 0.66f },
            { Params::spill, 0.22f }, { Params::defect, 0.58f }, { Params::gap, 0.65f }, { Params::bite, 0.68f }, { Params::mix, 1.0f },
        }},
        { "Permafrost Drop", {
            { Params::tight, 0.52f }, { Params::heat, 0.7f }, { Params::imprint, 0.68f }, { Params::relax, 0.28f },
            { Params::refractory, 0.38f }, { Params::spill, 0.3f }, { Params::core, 0.62f }, { Params::gap, 0.48f }, { Params::level, -3.0f },
        }},
        { "Quarantine Breakdown", {
            { Params::tight, 0.63f }, { Params::heat, 0.78f }, { Params::hardening, 0.82f }, { Params::crush, 0.7f },
            { Params::defect, 0.74f }, { Params::bite, 0.82f }, { Params::collapse, 0.73f }, { Params::mix, 0.92f }, { Params::level, -5.0f },
        }}
    };
}
}
