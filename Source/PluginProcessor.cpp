/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDistortionAudioProcessor::SimpleDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	p_distortion = std::make_unique<Distortion>(&apvts);
}

SimpleDistortionAudioProcessor::~SimpleDistortionAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleDistortionAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool SimpleDistortionAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool SimpleDistortionAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool SimpleDistortionAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double SimpleDistortionAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int SimpleDistortionAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDistortionAudioProcessor::getCurrentProgram()
{
	return 0;
}

void SimpleDistortionAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SimpleDistortionAudioProcessor::getProgramName(int index)
{
	return {};
}

void SimpleDistortionAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleDistortionAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	juce::dsp::ProcessSpec spec;

	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 1;
	spec.sampleRate = sampleRate;

	p_distortion->Prepare(spec);

	rmsLevelInputLeft.reset(sampleRate, 0.5f);
	rmsLevelInputRight.reset(sampleRate, 0.5f);
	rmsLevelOutputLeft.reset(sampleRate, 0.5f);
	rmsLevelOutputRight.reset(sampleRate, 0.5f);

	rmsLevelInputLeft.setCurrentAndTargetValue(-100.f);
	rmsLevelInputRight.setCurrentAndTargetValue(-100.f);
	rmsLevelOutputLeft.setCurrentAndTargetValue(-100.f);
	rmsLevelOutputRight.setCurrentAndTargetValue(-100.f);
}

void SimpleDistortionAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDistortionAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if(layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if(layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void SimpleDistortionAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	//Input LevelMeter calculations
	juce::ScopedNoDenormals noDenormals;
	rmsLevelInputLeft.skip(buffer.getNumSamples());
	rmsLevelInputRight.skip(buffer.getNumSamples());
	{
		const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
		if(value < rmsLevelInputLeft.getCurrentValue())
			rmsLevelInputLeft.setTargetValue(value);
		else
			rmsLevelInputLeft.setCurrentAndTargetValue(value);
	}
	{
		const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
		if(value < rmsLevelInputRight.getCurrentValue())
			rmsLevelInputRight.setTargetValue(value);
		else
			rmsLevelInputRight.setCurrentAndTargetValue(value);
	}

	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for(auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	juce::dsp::AudioBlock<float> block(buffer);
	juce::dsp::ProcessContextReplacing<float> context(block);
	p_distortion->Process(context);

	//Output LevelMeter calculations
	//juce::ScopedNoDenormals noDenormals;
	rmsLevelOutputLeft.skip(buffer.getNumSamples());
	rmsLevelOutputRight.skip(buffer.getNumSamples());
	{
		const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
		if(value < rmsLevelOutputLeft.getCurrentValue())
			rmsLevelOutputLeft.setTargetValue(value);
		else
			rmsLevelOutputLeft.setCurrentAndTargetValue(value);
	}
	{
		const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
		if(value < rmsLevelOutputRight.getCurrentValue())
			rmsLevelOutputRight.setTargetValue(value);
		else
			rmsLevelOutputRight.setCurrentAndTargetValue(value);
	}
}

//==============================================================================
bool SimpleDistortionAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleDistortionAudioProcessor::createEditor()
{
	return new SimpleDistortionAudioProcessorEditor(*this);
	//return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleDistortionAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

	juce::MemoryOutputStream mos(destData, true);
	apvts.state.writeToStream(mos);
}

void SimpleDistortionAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.

	auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
	if(tree.isValid())
	{
		apvts.replaceState(tree);
	}
}

float SimpleDistortionAudioProcessor::getRmsValue(const int channel, const bool isInput) const
{
	jassert(channel == 0 || channel == 1);
	if(channel == 0)
		if(isInput)
			return rmsLevelInputLeft.getCurrentValue();
		else
			return rmsLevelOutputLeft.getCurrentValue();
	if(channel == 1)
		if(isInput)
			return rmsLevelInputRight.getCurrentValue();
		else
			return rmsLevelOutputRight.getCurrentValue();
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SimpleDistortionAudioProcessor();
}

// copy "$(OutputPath)$(TargetFileName)" "D:\VSTBuilds\VST3$(TargetFileName)"