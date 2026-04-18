#include <cassert>
#include <cmath>
#include <iostream>

#include "dsp/MemristiveDrive.h"
#include "dsp/ChugField.h"
#include "dsp/DefectStage.h"
#include "dsp/CollapseState.h"

static void testMemristiveBounds()
{
    rc::dsp::MemristiveDrive d;
    juce::dsp::ProcessSpec spec{ 48000.0, 256, 1 };
    d.prepare(spec);

    rc::dsp::MemristiveDrive::Params p;
    p.heat = 1.0f;
    p.imprint = 1.0f;
    p.hardening = 1.0f;
    p.relax = 0.0f;
    d.setParameters(p);

    for (int i = 0; i < 20000; ++i)
        d.processSample(std::sin(0.01f * static_cast<float>(i)) * 0.9f);

    assert(d.getMemoryState() >= 0.0f && d.getMemoryState() <= 1.0f);
}

static void testChugFieldFinite()
{
    rc::dsp::ChugField c;
    c.prepare(48000.0, 16);
    rc::dsp::ChugField::Params p;
    p.ignition = 0.9f;
    p.spill = 0.8f;
    c.setParameters(p);

    std::array<float, rc::dsp::ChugField::NumBands> envs{};
    envs.fill(0.8f);

    for (int i = 0; i < 5000; ++i)
        c.processBlock(envs, 1);

    const auto avg = c.getAverageExcitation();
    assert(std::isfinite(avg));
}

static void testCollapseTransitions()
{
    rc::dsp::CollapseState s;
    s.prepare(48000.0);
    s.setAmount(1.0f);

    for (int i = 0; i < 5000; ++i)
        s.update(1.0f, 1.0f, 1.0f);

    assert(s.getMode() == rc::dsp::CollapseState::Mode::collapsed);
}

int main()
{
    testMemristiveBounds();
    testChugFieldFinite();
    testCollapseTransitions();

    std::cout << "RefractoryChamberDSPTests: PASS\n";
    return 0;
}
