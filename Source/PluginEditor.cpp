#include "PluginEditor.h"

RefractoryChamberAudioProcessorEditor::RefractoryChamberAudioProcessorEditor(RefractoryChamberAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), memoryMeter(memoryValue)
{
    setSize(560, 260);

    addSlider(heat, heatL, "Heat", rc::state::Params::heat);
    addSlider(imprint, imprintL, "Imprint", rc::state::Params::imprint);
    addSlider(hardening, hardeningL, "Hardening", rc::state::Params::hardening);
    addSlider(relax, relaxL, "Relax", rc::state::Params::relax);

    heatAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, rc::state::Params::heat, heat);
    imprintAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, rc::state::Params::imprint, imprint);
    hardeningAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, rc::state::Params::hardening, hardening);
    relaxAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, rc::state::Params::relax, relax);

    memoryLabel.setText("Memory", juce::dontSendNotification);
    memoryLabel.setColour(juce::Label::textColourId, juce::Colours::orangered);
    addAndMakeVisible(memoryLabel);
    addAndMakeVisible(memoryMeter);

    startTimerHz(24);
}

void RefractoryChamberAudioProcessorEditor::addSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String&)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
    label.setText(text, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

void RefractoryChamberAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111114));
    g.setColour(juce::Colour(0xffc1462e));
    g.setFont(20.0f);
    g.drawText("Refractory Chamber", 16, 8, getWidth() - 32, 24, juce::Justification::centredLeft);
}

void RefractoryChamberAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16);
    area.removeFromTop(34);
    auto knobs = area.removeFromTop(160);

    const int w = knobs.getWidth() / 4;
    heat.setBounds(knobs.removeFromLeft(w).reduced(8));
    imprint.setBounds(knobs.removeFromLeft(w).reduced(8));
    hardening.setBounds(knobs.removeFromLeft(w).reduced(8));
    relax.setBounds(knobs.removeFromLeft(w).reduced(8));

    auto meterArea = area.removeFromTop(24);
    memoryLabel.setBounds(meterArea.removeFromLeft(80));
    memoryMeter.setBounds(meterArea.reduced(4, 2));
}

void RefractoryChamberAudioProcessorEditor::timerCallback()
{
    memoryValue = processor.getMemoryMeter();
}
