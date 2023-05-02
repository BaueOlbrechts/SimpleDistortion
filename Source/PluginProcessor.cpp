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
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
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
	juce::ScopedNoDenormals noDenormals;
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

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.

	auto chainSettings = getChainSettings(apvts);

	buffer.applyGain(juce::Decibels::decibelsToGain(chainSettings.inputGainInDecibels));

	for(int channel = 0; channel < totalNumInputChannels; ++channel)
	{

		auto* channelData = buffer.getWritePointer(channel);

		for(int i{ 0 }; i < buffer.getNumSamples(); i++)
		{
			auto input = channelData[i];

			channelData[i] = input;
		}


	}
	buffer.applyGain(juce::Decibels::decibelsToGain(chainSettings.outputGainInDecibels));

}

//==============================================================================
bool SimpleDistortionAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleDistortionAudioProcessor::createEditor()
{
	//return new SimpleDistortionAudioProcessorEditor(*this);
	return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleDistortionAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void SimpleDistortionAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//Custom stuff under here
constexpr const char* ClippingTypeToString(SimpleDistortionAudioProcessor::ClippingType ct) throw()
{
	switch(ct)
	{
		case SimpleDistortionAudioProcessor::ClippingType::SoftClipping:
			return "Soft Clipping";
		case SimpleDistortionAudioProcessor::ClippingType::HardClipping:
			return "Hard Clipping";
		case SimpleDistortionAudioProcessor::ClippingType::Dummy1:
			return "Dummy 1";
		case SimpleDistortionAudioProcessor::ClippingType::Dummy2:
			return "Dummy 1";
		default:
			throw std::invalid_argument("Unimplemented clippingtype");
	}
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
	ChainSettings settings;

	settings.inputGainInDecibels = apvts.getRawParameterValue("Input Gain")->load();
	settings.outputGainInDecibels = apvts.getRawParameterValue("Output Gain")->load());
	settings.mix = apvts.getRawParameterValue("Mix")->load();
	settings.drive = apvts.getRawParameterValue("Drive/Hardness")->load();
	settings.clippingType = apvts.getRawParameterValue("Clipping Type")->load();

	return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleDistortionAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		"Drive/Hardness",
		"Drive/Hardness",
		juce::NormalisableRange<float>(0.0f, 1.0f),
		0.5f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		"Input Gain",
		"Input Gain",
		juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
		0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		"Output Gain",
		"Output Gain",
		juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
		0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		"Mix",
		"Mix",
		juce::NormalisableRange<float>(0.0f, 1.0f),
		0.5f));


	juce::StringArray clippingTypeArray(ClippingTypeToString(ClippingType::HardClipping),
		ClippingTypeToString(ClippingType::SoftClipping),
		ClippingTypeToString(ClippingType::Dummy1),
		ClippingTypeToString(ClippingType::Dummy2));

	layout.add(std::make_unique<juce::AudioParameterChoice>(
		"Clipping Type",
		"Clipping Type",
		clippingTypeArray,
		0));


	return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SimpleDistortionAudioProcessor();
}
