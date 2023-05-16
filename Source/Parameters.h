#ifndef PARAMETERS_H
#define PARAMETERS_H

#pragma once
#include <JuceHeader.h>



class Parameters
{
public:
	inline static const juce::String
		ID_INPUT = "input",
		ID_INPUT_DISPLAY = "Input Gain",
		ID_OUTPUT = "output",
		ID_OUTPUT_DISPLAY = "Output Gain",
		ID_MIX = "mix",
		ID_MIX_DISPLAY = "Mix",
		ID_DRIVE = "drive",
		ID_DRIVE_DISPLAY = "Drive/Hardness",
		ID_CLIPPINGTYPE = "clippingType",
		ID_CLIPPINGTYPE_DISPLAY = "Clipping Type",
		ID_BYPASS = "bypass",
		ID_BYPASS_DISPLAY = "Bypass";

	enum class ClippingType
	{
		SoftClipping,
		HardClipping,
		SineFold,
		Dummy2
	};

	static const char* ClippingTypeToString(ClippingType ct) noexcept
	{
		switch(ct)
		{
			case ClippingType::SoftClipping:
				return "Soft Clipping";
			case ClippingType::HardClipping:
				return "Hard Clipping";
			case ClippingType::SineFold:
				return "Sine Fold";
			case ClippingType::Dummy2:
				return "Dummy 2";
		}
	}

	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
	{
		juce::AudioProcessorValueTreeState::ParameterLayout layout;

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			ID_DRIVE,
			ID_DRIVE_DISPLAY,
			juce::NormalisableRange<float>(0.0f, 1.0f),
			0.5f));

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			ID_INPUT,
			ID_INPUT_DISPLAY,
			juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
			0.0f));

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			ID_OUTPUT,
			ID_OUTPUT_DISPLAY,
			juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
			0.0f));

		layout.add(std::make_unique<juce::AudioParameterFloat>(
			ID_MIX,
			ID_MIX_DISPLAY,
			juce::NormalisableRange<float>(0.0f, 1.0f),
			1.0f));


		juce::StringArray clippingTypeArray(ClippingTypeToString(ClippingType::SoftClipping),
			ClippingTypeToString(ClippingType::HardClipping),
			ClippingTypeToString(ClippingType::SineFold),
			ClippingTypeToString(ClippingType::Dummy2));

		layout.add(std::make_unique<juce::AudioParameterChoice>(
			ID_CLIPPINGTYPE,
			ID_CLIPPINGTYPE_DISPLAY,
			clippingTypeArray,
			0));

		layout.add(std::make_unique<juce::AudioParameterBool>(
			ID_BYPASS,
			ID_BYPASS_DISPLAY,
			false));


		return layout;
	}
	
	struct ChainSettings
	{
		float inputGainInDecibels{ 0.0f }, mix{ 0.5f }, outputGainInDecibels{ 0.0f }, drive{ 0.5f };
		ClippingType clippingType{ ClippingType::SoftClipping };
	};

	static ChainSettings getChainSettings(juce::AudioProcessorValueTreeState* apvts)
	{
		ChainSettings settings;

		settings.inputGainInDecibels = apvts->getRawParameterValue(ID_INPUT)->load();
		settings.outputGainInDecibels = apvts->getRawParameterValue(ID_OUTPUT)->load();
		settings.mix = apvts->getRawParameterValue(ID_MIX)->load();
		settings.drive = apvts->getRawParameterValue(ID_DRIVE)->load();
		settings.clippingType = static_cast<ClippingType>(apvts->getRawParameterValue(ID_CLIPPINGTYPE)->load());

		return settings;
	}
};
#endif
