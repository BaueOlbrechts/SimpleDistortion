#ifndef DISTORTION_H
#define DISTORTION_H

#pragma once
#include "JuceHeader.h"
#include "Parameters.h"

using DataType = float;
using ProcessContext = juce::dsp::ProcessContextReplacing<DataType>;

class Distortion
{
public:
	Distortion(juce::AudioProcessorValueTreeState* p_apvts);
	~Distortion();

	void Prepare(const juce::dsp::ProcessSpec& spec);
	void Process(const ProcessContext& context) noexcept;
	void Reset() noexcept;

private:
	enum
	{
		waveShaperIndex,
		postGainIndex,
	};
	juce::dsp::ProcessorChain<juce::dsp::WaveShaper<DataType, std::function<DataType(DataType)>>, juce::dsp::Gain<DataType>> processorChain;
	juce::AudioProcessorValueTreeState* p_apvts;

	std::function<DataType(DataType)> GetDistortionAlgorithm(Parameters::ClippingType clippingType, float drive, float hardness, float mix);
	void UpdateChain();
};

#endif
