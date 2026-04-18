#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "dsp/InputStage.h"
#include "dsp/Oversampler.h"
#include "dsp/MemristiveDrive.h"
#include "dsp/BandSplitter.h"
#include "dsp/ChugField.h"
#include "dsp/DefectStage.h"
#include "dsp/CollapseState.h"
#include "state/ParameterLayout.h"
#include "state/PresetManager.h"

class RefractoryChamberAudioProcessor final : public juce::AudioProcessor
{
public:
    RefractoryChamberAudioProcessor();
    ~RefractoryChamberAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    float getMemoryMeter() const noexcept { return memoryMeter.load(); }

    juce::AudioProcessorValueTreeState apvts;

private:
    float getParam(const juce::String& id) const;

    rc::dsp::InputStage inputStage;
    rc::dsp::Oversampler oversampler;
    rc::dsp::MemristiveDrive memristive;
    rc::dsp::BandSplitter bandSplitter;
    rc::dsp::ChugField chugField;
    rc::dsp::DefectStage defectStage;
    rc::dsp::CollapseState collapseState;

    juce::AudioBuffer<float> monoBuffer;
    std::array<float, 4> modState{ 1.0f, 1.0f, 1.0f, 1.0f };
    std::atomic<float> memoryMeter{ 0.0f };

    std::vector<rc::state::FactoryPreset> presets;
    int currentProgram{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RefractoryChamberAudioProcessor)
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
