/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <complex>

static const int MINIMUM_BUFFER_SIZE = 32;
static const float BUFFER_OFFSET_FACTOR = 1.5;
static const kiss_fft_cpx zeroComplex = { 0.0f, 0.0f };
//==============================================================================
FrequencyLimiterAudioProcessor::FrequencyLimiterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	),
#endif
	parameters(*this, nullptr, Identifier("FrequencyCleaner"),
		{ std::make_unique<AudioParameterFloat>("threshold", "Threshold", NormalisableRange<float>(-100.0f, 0.0f, 0.1f), 0.0f),
		std::make_unique<AudioParameterFloat>("gain", "Makeup Gain", NormalisableRange<float>(0.0f, 20.0f, 0.1f), 0.0f),
		std::make_unique<AudioParameterFloat>("mix", "Mix", NormalisableRange<float>(0.0f, 100.0f, 1.0f), 100.0f) }),
	window(1024, dsp::WindowingFunction<float>::hann)
{
	threshold = parameters.getRawParameterValue("threshold");
	gain = parameters.getRawParameterValue("gain");
	mix = parameters.getRawParameterValue("mix");
	cfgFFT = NULL;
	cfgIFFT = NULL;
}

FrequencyLimiterAudioProcessor::~FrequencyLimiterAudioProcessor()
{
	kiss_fftr_free(cfgFFT);
	kiss_fftr_free(cfgIFFT);
}

//==============================================================================
const String FrequencyLimiterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FrequencyLimiterAudioProcessor::acceptsMidi() const
{
	return false;
}

bool FrequencyLimiterAudioProcessor::producesMidi() const
{
	return false;
}

bool FrequencyLimiterAudioProcessor::isMidiEffect() const
{
	return false;
}

double FrequencyLimiterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FrequencyLimiterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FrequencyLimiterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FrequencyLimiterAudioProcessor::setCurrentProgram (int index)
{
}

const String FrequencyLimiterAudioProcessor::getProgramName (int index)
{
    return {};
}

void FrequencyLimiterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FrequencyLimiterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	int N = (int)((BUFFER_OFFSET_FACTOR * samplesPerBlock) + .5);
	if (N <= MINIMUM_BUFFER_SIZE) N = MINIMUM_BUFFER_SIZE;
	if (N % 2 == 1) N += 1;
	int K = N / 2 + 1;
	realBufferFFT.resize(N, 0.0f);
	complexBufferFFT.resize(K, zeroComplex);
	cfgFFT = kiss_fftr_alloc(N, 0, NULL, NULL);
	cfgIFFT = kiss_fftr_alloc(N, 1, NULL, NULL);
	oldSamplesPerBlock = N;

	//windowing function
	sidechainBufferFFT.resize(K, zeroComplex);
	window.fillWindowingTables(N, juce::dsp::WindowingFunction<float>::hann);
}

void FrequencyLimiterAudioProcessor::releaseResources()
{
	if (cfgFFT != NULL)
	{
		free(cfgFFT);
		cfgFFT = NULL;
	}
	if (cfgIFFT != NULL)
	{
		free(cfgIFFT);
		cfgIFFT = NULL;
	}

	std::fill(realBufferFFT.begin(), realBufferFFT.end(), 0.0f);
	std::fill(complexBufferFFT.begin(), complexBufferFFT.end(), zeroComplex);
	std::fill(sidechainBufferFFT.begin(), sidechainBufferFFT.end(), zeroComplex);
}

bool FrequencyLimiterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;
	else return true;
}

void FrequencyLimiterAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	int bufferSize = buffer.getNumSamples();
	int N = oldSamplesPerBlock;
	int K = N / 2 + 1;
	const float thresholdValue = Decibels::decibelsToGain(*threshold);
	const float gainValue = Decibels::decibelsToGain(*gain);
	float sidechainRadius;
	float mainRadius;
	float tempAngle;
	float newRadius;
	const float mixValue = *mix / 100.0f;
	float newSampleValue;
	std::complex<float> tempComplex;
	if (N < bufferSize)
	{
		N = (int)((BUFFER_OFFSET_FACTOR * bufferSize) + .5);
		if (N <= MINIMUM_BUFFER_SIZE) N = MINIMUM_BUFFER_SIZE;
		if (N % 2 == 1) N += 1;
		K = N / 2 + 1;
		realBufferFFT.resize(N, 0.0f);
		complexBufferFFT.resize(K);
		free(cfgFFT);
		free(cfgIFFT);
		cfgFFT = kiss_fftr_alloc(N, 0, NULL, NULL);
		cfgIFFT = kiss_fftr_alloc(N, 1, NULL, NULL);
		oldSamplesPerBlock = N;
	}

	std::fill(realBufferFFT.begin(), realBufferFFT.end(), 0.0f);

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		for (int i = 0; i < bufferSize; ++i) realBufferFFT[i] = buffer.getSample(channel, i);
		kiss_fftr(cfgFFT, &realBufferFFT[0], &complexBufferFFT[0]);
		window.multiplyWithWindowingTable(&realBufferFFT[0], N);
		kiss_fftr(cfgFFT, &realBufferFFT[0], &sidechainBufferFFT[0]);
		for (int i = 0; i < K; ++i)
		{
			mainRadius = std::sqrt(std::pow(std::abs(complexBufferFFT[i].r), 2) + std::pow(std::abs(complexBufferFFT[i].i), 2)) / N;
			sidechainRadius = std::sqrt(std::pow(std::abs(sidechainBufferFFT[i].r), 2) + std::pow(std::abs(sidechainBufferFFT[i].i), 2)) / N;
			tempAngle = atan2(complexBufferFFT[i].i, complexBufferFFT[i].r);
			newRadius = (sidechainRadius > thresholdValue) ? thresholdValue : mainRadius;
			tempComplex = std::polar(newRadius, tempAngle);
			complexBufferFFT[i].r = std::real(tempComplex);
			complexBufferFFT[i].i = std::imag(tempComplex);
		}
		kiss_fftri(cfgIFFT, &complexBufferFFT[0], &realBufferFFT[0]);
		for (int i = 0; i < bufferSize; ++i)
		{
			newSampleValue = (mixValue * realBufferFFT[i]) + ((1 - mixValue) * buffer.getSample(channel, i));
			buffer.setSample(channel, i, newSampleValue);
		}
	}
	if (gainValue != previousGain)
	{
		buffer.applyGainRamp(0, bufferSize, previousGain, gainValue);
		previousGain = gainValue;
	}
	else
	{
		buffer.applyGain(gainValue);
	}
}

//==============================================================================
bool FrequencyLimiterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FrequencyLimiterAudioProcessor::createEditor()
{
    return new FrequencyLimiterAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void FrequencyLimiterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	XmlElement * xml = new XmlElement("GainStager");
	xml->setAttribute(Identifier("thresholdParameter"), (double)parameters.getParameter("threshold")->convertTo0to1(*threshold));
	xml->setAttribute(Identifier("mixParameter"), (double)parameters.getParameter("mix")->convertTo0to1(*mix));
	xml->setAttribute(Identifier("gainParameter"), (double)parameters.getParameter("gain")->convertTo0to1(*gain));
	copyXmlToBinary(*xml, destData);
	delete xml;
}

void FrequencyLimiterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState = getXmlFromBinary(data, sizeInBytes);
	if (xmlState != nullptr)
	{
		const float thresholdValue = (float)xmlState->getDoubleAttribute("thresholdParameter", 1.0f);
		const float gainValue = (float)xmlState->getDoubleAttribute("gainParameter", 0.0f);
		const float mixValue = (float)xmlState->getDoubleAttribute("mixParameter", 1.0f);

		parameters.getParameter("threshold")->beginChangeGesture();
		parameters.getParameter("gain")->beginChangeGesture();
		parameters.getParameter("mix")->beginChangeGesture();

		parameters.getParameter("threshold")->setValueNotifyingHost(thresholdValue);
		parameters.getParameter("gain")->setValueNotifyingHost(gainValue);
		parameters.getParameter("mix")->setValueNotifyingHost(mixValue);

		parameters.getParameter("threshold")->endChangeGesture();
		parameters.getParameter("gain")->endChangeGesture();
		parameters.getParameter("mix")->endChangeGesture();
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FrequencyLimiterAudioProcessor();
}
