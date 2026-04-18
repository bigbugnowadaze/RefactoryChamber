#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class RefractoryChamberAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                    private juce::Timer
{
public:
    explicit RefractoryChamberAudioProcessorEditor(RefractoryChamberAudioProcessor&);
    ~RefractoryChamberAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void addSlider(juce::Slider&, juce::Label&, const juce::String&, const juce::String&);

    RefractoryChamberAudioProcessor& processor;

    juce::Slider heat, imprint, hardening, relax;
    juce::Label heatL, imprintL, hardeningL, relaxL, memoryLabel;
    juce::ProgressBar memoryMeter;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> heatAtt, imprintAtt, hardeningAtt, relaxAtt;
    double memoryValue{ 0.0 };
};
