/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
//==============================================================================
/**
*/
class FrequencyLimiterAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener, public ComboBox::Listener
{
public:
    FrequencyLimiterAudioProcessorEditor (FrequencyLimiterAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~FrequencyLimiterAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void sliderValueChanged(Slider * slider) override;
	void comboBoxChanged(ComboBox * comboBox) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FrequencyLimiterAudioProcessor& processor;
	AudioProcessorValueTreeState& valueTreeState;
	ScopedPointer<Slider> thresholdSlider;
	ScopedPointer<SliderAttachment> thresholdSliderAttachment;
	ScopedPointer<Slider> gainSlider;
	ScopedPointer<SliderAttachment> gainSliderAttachment;
	ScopedPointer<Slider> mixSlider;
	ScopedPointer<SliderAttachment> mixSliderAttachment;
	ScopedPointer<Label> thresholdLabel;
	ScopedPointer<Label> gainLabel;
	ScopedPointer<Label> mixLabel;
	ScopedPointer<ComboBox> windowComboBox;
	ScopedPointer<ComboBoxAttachment> windowComboBoxAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyLimiterAudioProcessorEditor)
};
