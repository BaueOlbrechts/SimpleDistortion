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
	waveshaper.functionToUse = GetDistortionAlgorithm(settings.clippingType, juce::Decibels::decibelsToGain(settings.driveInDecibels), settings.hardness, settings.mix);

	auto& postGain = processorChain.template get<postGainIndex>();
	postGain.setGainDecibels(settings.outputGainInDecibels);
}

std::function<DataType(DataType)> Distortion::GetDistortionAlgorithm(Parameters::ClippingType clippingType, float drive, float hardness, float mix)
{
	/*
	* Parameter ranges
	* drive in db [-30,30]
	* theoretical drive in gain ]0, +inf[
	* hardness [0,1]
	* mix [0.1]
	* x [-1,1]
	*/

	switch(clippingType)
	{
		case Parameters::ClippingType::SoftClipping:
			return [drive, hardness, mix](DataType x)
			{
				float cleanInput{ x };
				float driveInput{ cleanInput * drive };
				float hardnessConstant{ hardness * 10 };

				return (2.0f / 3.1415f *hardnessConstant * atan(driveInput) * mix + cleanInput * (1 - mix));
			};
			break;
		case Parameters::ClippingType::HardClipping:
			return [drive, hardness, mix](DataType x)
			{
				float cleanInput{ x };
				float driveInput{ cleanInput * drive };
				float hardnessConstant{ std::clamp(1.0f - hardness, 0.05f, 1.0f) }; //prevent from going to 0

				return juce::jlimit(float(-hardnessConstant), float(hardnessConstant), driveInput) * 1 / hardnessConstant * mix + cleanInput * (1 - mix);
			};
			break;
		case Parameters::ClippingType::SineFold:
			return [drive, hardness, mix](DataType x)
			{
				float cleanInput{ x };
				float driveInput{ cleanInput * drive };
				float maxFolds{ 5.f }, minFolds{ 0.5f };
				float hardnessConstant{ hardness * (maxFolds - minFolds) + minFolds };

				return sin(driveInput * hardnessConstant) * mix + cleanInput * (1 - mix);
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





