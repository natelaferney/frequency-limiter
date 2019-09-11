/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
extern "C" {
#include "KissFFT/kiss_fft.h"
}
extern "C" {
#include "KissFFT/tools/kiss_fftr.h"
}
#include <vector>

//==============================================================================
/**
*/
class FrequencyLimiterAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FrequencyLimiterAudioProcessor();
    ~FrequencyLimiterAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
	AudioProcessorValueTreeState parameters;
	float * threshold;
	float * gain;
	float * mix;
	float previousGain{ 1.0f };
	std::vector<kiss_fft_scalar> realBufferFFT;
	std::vector<kiss_fft_cpx> complexBufferFFT;
	kiss_fftr_cfg cfgFFT;
	kiss_fftr_cfg cfgIFFT;
	int oldSamplesPerBlock{ 0 };
	dsp::WindowingFunction<float> window;
	std::vector<kiss_fft_cpx> sidechainBufferFFT;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyLimiterAudioProcessor)
};
