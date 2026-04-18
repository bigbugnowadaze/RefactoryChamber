#include "PluginProcessor.h"
#include "PluginEditor.h"

RefractoryChamberAudioProcessor::RefractoryChamberAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::mono(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Params", rc::state::createParameterLayout()),
      presets(rc::state::getFactoryPresets())
{
}

void RefractoryChamberAudioProcessor::prepareToPlay(double sr, int maxBlock)
{
    juce::dsp::ProcessSpec spec{ sr, static_cast<juce::uint32>(maxBlock), static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    inputStage.prepare(spec);
    memristive.prepare(spec);
    defectStage.prepare(spec);
    bandSplitter.prepare({ sr, static_cast<juce::uint32>(maxBlock), 1u });
    chugField.prepare(sr, 32);
    collapseState.prepare(sr);
    oversampler.prepare(static_cast<size_t>(getTotalNumOutputChannels()), static_cast<size_t>(maxBlock));
    monoBuffer.setSize(1, maxBlock, false, false, true);
    modState = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void RefractoryChamberAudioProcessor::releaseResources() {}

float RefractoryChamberAudioProcessor::getParam(const juce::String& id) const
{
    if (auto* p = apvts.getRawParameterValue(id))
        return p->load();
    return 0.0f;
}

int RefractoryChamberAudioProcessor::getNumPrograms()
{
    return static_cast<int>(presets.size());
}

const juce::String RefractoryChamberAudioProcessor::getProgramName(int index)
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[static_cast<size_t>(index)].name;
    return {};
}

void RefractoryChamberAudioProcessor::setCurrentProgram(int index)
{
    if (index < 0 || index >= static_cast<int>(presets.size()))
        return;

    currentProgram = index;
    const auto& preset = presets[static_cast<size_t>(index)];
    for (const auto& [id, value] : preset.values)
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
}

void RefractoryChamberAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    inputStage.setParameters(getParam(rc::state::Params::gate), getParam(rc::state::Params::tight), getParam(rc::state::Params::punch));
    inputStage.process(buffer);

    oversampler.setMode(static_cast<int>(getParam(rc::state::Params::oversampling)));

    rc::dsp::MemristiveDrive::Params md;
    md.heat = getParam(rc::state::Params::heat);
    md.imprint = getParam(rc::state::Params::imprint);
    md.hardening = getParam(rc::state::Params::hardening);
    md.relax = getParam(rc::state::Params::relax);
    memristive.setParameters(md);

    rc::dsp::ChugField::Params cf;
    cf.ignition = getParam(rc::state::Params::ignition);
    cf.refractory = getParam(rc::state::Params::refractory);
    cf.spill = getParam(rc::state::Params::spill);
    cf.crush = getParam(rc::state::Params::crush);
    chugField.setParameters(cf);

    rc::dsp::DefectStage::Params ds;
    ds.core = getParam(rc::state::Params::core);
    ds.defect = getParam(rc::state::Params::defect);
    ds.gap = getParam(rc::state::Params::gap);
    ds.bite = getParam(rc::state::Params::bite);
    defectStage.setParameters(ds);

    collapseState.setAmount(getParam(rc::state::Params::collapse));

    auto dry = buffer;
    oversampler.process(buffer, [this](auto upBlock)
    {
        memristive.processBlock(upBlock);
    });

    monoBuffer.clear();
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    juce::AudioBuffer<float> monoView(monoBuffer.getArrayOfWritePointers(), 1, buffer.getNumSamples());
    bandSplitter.process(monoView);

    const auto& envs = bandSplitter.getEnvelopes();
    chugField.processBlock(envs, buffer.getNumSamples());

    const auto mods = chugField.getBandModulators();
    const float fieldAvg = chugField.getAverageExcitation();
    collapseState.update(memristive.getMemoryState(), fieldAvg, envs[0]);
    const float collapseBoost = collapseState.getDriveBoost();

    for (size_t i = 0; i < modState.size(); ++i)
        modState[i] += 0.25f * (mods[i] - modState[i]);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            const size_t band = static_cast<size_t>((n * 4) / juce::jmax(1, buffer.getNumSamples()));
            const float x = data[n] * modState[juce::jmin<size_t>(band, 3)] * collapseBoost;
            data[n] = defectStage.processSample(x, memristive.getMemoryState(), fieldAvg);
        }
    }

    const float wet = getParam(rc::state::Params::mix);
    const float dryAmt = 1.0f - wet;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        buffer.addFrom(ch, 0, dry, ch, 0, buffer.getNumSamples(), dryAmt);

    buffer.applyGain(juce::Decibels::decibelsToGain(getParam(rc::state::Params::level)));
    memoryMeter.store(memristive.getMemoryState());
}

juce::AudioProcessorEditor* RefractoryChamberAudioProcessor::createEditor()
{
    return new RefractoryChamberAudioProcessorEditor(*this);
}

void RefractoryChamberAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("currentProgram", currentProgram, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RefractoryChamberAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        auto tree = juce::ValueTree::fromXml(*xmlState);
        apvts.replaceState(tree);
        setCurrentProgram(static_cast<int>(tree.getProperty("currentProgram", 0)));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RefractoryChamberAudioProcessor();
}
