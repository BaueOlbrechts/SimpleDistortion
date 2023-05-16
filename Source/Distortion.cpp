#pragma once
#include "Distortion.h"

Distortion::Distortion(juce::AudioProcessorValueTreeState* apvts) :
	p_apvts{ apvts }
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
	UpdateChain();
	processorChain.process(context);
}
void Distortion::Reset() noexcept
{
	processorChain.reset();
}

void Distortion::UpdateChain()
{
	Parameters::ChainSettings settings = Parameters::getChainSettings(p_apvts);

	auto& waveshaper = processorChain.template get<waveShaperIndex>();
	waveshaper.functionToUse = GetDistortionAlgorithm(settings.clippingType, settings.drive, settings.hardness, settings.mix);

	auto& postGain = processorChain.template get<postGainIndex>();
	postGain.setGainDecibels(settings.outputGainInDecibels);
}

std::function<DataType(DataType)> Distortion::GetDistortionAlgorithm(Parameters::ClippingType clippingType, float drive, float hardness, float mix)
{
	switch(clippingType)
	{
		case Parameters::ClippingType::SoftClipping:
			return [drive, mix](DataType x)
			{
				return (2.0f / 3.1415f * atan(drive * x) * mix + x * (1 - mix));
				//return copysignf(x, 1 - std::pow(std::exp(1.0f), -drive * std::abs(x)));
				//return atan(drive * x);
			};
			break;
		case Parameters::ClippingType::HardClipping:
			return [drive, mix](DataType x) { return juce::jlimit(float(drive - 1.01f), float(1.01f - drive), x) * mix + x * (1 - mix); };
			break;
		case Parameters::ClippingType::SineFold:
			return [drive, mix](DataType x)
			{
				float maxFolds = 10.f;
				return sin((drive * maxFolds + 1) * x) * mix + x * (1 - mix);
			};
			break;
		case Parameters::ClippingType::Dummy2:
			break;
		default:
			return [](DataType x) { return x; };
			break;
	}

	return [](DataType x) { return x; };
}





