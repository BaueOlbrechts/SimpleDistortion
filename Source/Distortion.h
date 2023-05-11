#ifndef DISTORTION_H
#define DISTORTION_H

#pragma once
#include "JuceHeader.h"
#include "Parameters.h"

using WaveShaperDataType = float;
using ProcessContext = juce::dsp::ProcessContextReplacing<WaveShaperDataType>;

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
		preGainIndex,
		waveShaperIndex,
		postGainIndex,
	};
	juce::dsp::ProcessorChain<juce::dsp::Gain<WaveShaperDataType>, juce::dsp::WaveShaper<WaveShaperDataType>, juce::dsp::Gain<WaveShaperDataType>> processorChain;
	juce::AudioProcessorValueTreeState* p_apvts;

	void UpdateChain();
};

#endif
