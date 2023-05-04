/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include "Distortion.h"


//==============================================================================
/**
*/
class SimpleDistortionAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
	, public juce::AudioProcessorARAExtension
#endif
{
public:
	//==============================================================================
	SimpleDistortionAudioProcessor();
	~SimpleDistortionAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//Custom stuff under here


	juce::AudioProcessorValueTreeState apvts{ *this, nullptr,"Parameters", Parameters::createParameterLayout() };

private:
	Distortion* p_distortion = new Distortion;

	using WaveShaper = juce::dsp::WaveShaper<float>;
	juce::dsp::ProcessorChain<WaveShaper> processorChain;


	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDistortionAudioProcessor)
};

struct ChainSettings
{
	float inputGainInDecibels{ 0.0f }, mix{ 0.5f }, outputGainInDecibels{ 0.0f }, drive{ 0.5f };
	Parameters::ClippingType clippingType{ Parameters::ClippingType::SoftClipping };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);