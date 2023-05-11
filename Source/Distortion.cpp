#pragma once
#include "Distortion.h"

Distortion::Distortion(juce::AudioProcessorValueTreeState* apvts):
	p_apvts{apvts}
{
	UpdateChain();
}
Distortion::~Distortion()
{
}

void Distortion::Prepare(const juce::dsp::ProcessSpec& spec)
{
	processorChain.prepare(spec);
}
void Distortion::Process(const ProcessContext& context) noexcept
{

}
void Distortion::Reset() noexcept
{
	processorChain.reset();
}


void Distortion::UpdateChain()
{
	//ChainSettings settings = Parameters::getChainSettings(p_apvts);

	auto& waveshaper = processorChain.template get<waveShaperIndex>();
	waveshaper.functionToUse = [](float x)
	{
		return juce::jlimit(float(-0.3), float(0.3), x);
	};

	auto& preGain = processorChain.template get<preGainIndex>();
	preGain.setGainDecibels(30.0f);

	auto& postGain = processorChain.template get<postGainIndex>();
	postGain.setGainDecibels(-20.0f);
}



