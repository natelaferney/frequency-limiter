/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

static const Array<String> windowChoices = { "Rectangular", "Triangular", "Hann", "Hamming", "Blackman", "Blackman-Harris",
"Flat Top" };

//==============================================================================
FrequencyLimiterAudioProcessorEditor::FrequencyLimiterAudioProcessorEditor (FrequencyLimiterAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	setSize(270, 550);

	addAndMakeVisible(thresholdSlider = new Slider());
	thresholdSlider->setBounds(35, 40, 50, 400);
	thresholdSlider->setSliderStyle(Slider::LinearVertical);
	thresholdSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	thresholdSlider->addListener(this);
	thresholdSliderAttachment.reset(new SliderAttachment(valueTreeState, "threshold", *thresholdSlider));
	addAndMakeVisible(thresholdLabel = new Label());
	thresholdLabel->setBounds(25, 425, 100, 50);
	thresholdLabel->setText("Threshold", dontSendNotification);

	addAndMakeVisible(mixSlider = new Slider());
	mixSlider->setBounds(115, 40, 50, 400);
	mixSlider->setSliderStyle(Slider::LinearVertical);
	mixSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	mixSlider->addListener(this);
	mixSliderAttachment.reset(new SliderAttachment(valueTreeState, "mix", *mixSlider));
	addAndMakeVisible(mixLabel = new Label());
	mixLabel->setBounds(125, 425, 100, 50);
	mixLabel->setText("Mix", dontSendNotification);

	addAndMakeVisible(gainSlider = new Slider());
	gainSlider->setBounds(195, 40, 50, 400);
	gainSlider->setSliderStyle(Slider::LinearVertical);
	gainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	gainSlider->addListener(this);
	gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", *gainSlider));
	addAndMakeVisible(gainLabel = new Label());
	gainLabel->setBounds(202, 425, 100, 50);
	gainLabel->setText("Gain", dontSendNotification);

	addAndMakeVisible(windowComboBox = new ComboBox());
	windowComboBox->setBounds(20, 500, 100, 26);
	for (int i = 0; i < windowChoices.size(); ++i)
	{
		windowComboBox->addItem(windowChoices[i], i + 1);
	}
	windowComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "window", *windowComboBox));
	addAndMakeVisible(windowComboBoxLabel = new Label());
	windowComboBoxLabel->setBounds(20, 465, 100, 50);
	windowComboBoxLabel->setText("Window", dontSendNotification);
}

FrequencyLimiterAudioProcessorEditor::~FrequencyLimiterAudioProcessorEditor()
{
}

//==============================================================================
void FrequencyLimiterAudioProcessorEditor::paint (Graphics& g)
{
	ColourGradient gradient = ColourGradient(Colours::slategrey, 0.0f, 100.0f, Colours::darkslategrey, 200.0f, 500.0f, false);
	g.setGradientFill(gradient);
	g.fillAll();
}

void FrequencyLimiterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void FrequencyLimiterAudioProcessorEditor::sliderValueChanged(Slider * slider)
{

}

void FrequencyLimiterAudioProcessorEditor::comboBoxChanged(ComboBox * comboBox)
{

}