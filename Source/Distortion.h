#ifndef DISTORTION_H
#define DISTORTION_H

#pragma once
#include "JuceHeader.h"

using WaveShaperDataType = float;
using ProcessContext = juce::dsp::ProcessContextReplacing<WaveShaperDataType>;

class Distortion
{
public:
	Distortion();
	~Distortion();

	void Prepare(const juce::dsp::ProcessSpec& spec);
	void Process(const ProcessContext& context) noexcept;
	void Reset() noexcept;

	juce::dsp::ProcessorChain<juce::dsp::WaveShaper<WaveShaperDataType>> processorChain;
	
	int test{ 0 };
private:

};

#endif
