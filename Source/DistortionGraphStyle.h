#ifndef DISTORTIONGRAPHSTYLE_H
#define DISTORTIONGRAPHSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"
#include "Parameters.h"

class DistortionGraph : public juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
public:
	DistortionGraph(juce::AudioProcessorValueTreeState* apvts, std::function<float()>&& valueFunctionLeft, std::function<float()>&& valueFunctionRight) :
		p_apvts{ apvts },
		valueSupplierLeft(std::move(valueFunctionLeft)),
		valueSupplierRight(std::move(valueFunctionRight))
	{
		startTimerHz(24);

		p_apvts->getParameter(Parameters::ID_CLIPPINGTYPE)->addListener(this);
		p_apvts->getParameter(Parameters::ID_HARDNESS)->addListener(this);
		p_apvts->getParameter(Parameters::ID_MIX)->addListener(this);
		p_apvts->getParameter(Parameters::ID_OUTPUT)->addListener(this);
		p_apvts->getParameter(Parameters::ID_DRIVE)->addListener(this);

		updateGraph();
	};

	~DistortionGraph()
	{
		p_apvts->getParameter(Parameters::ID_CLIPPINGTYPE)->removeListener(this);
		p_apvts->getParameter(Parameters::ID_HARDNESS)->removeListener(this);
		p_apvts->getParameter(Parameters::ID_MIX)->removeListener(this);
		p_apvts->getParameter(Parameters::ID_OUTPUT)->removeListener(this);
		p_apvts->getParameter(Parameters::ID_DRIVE)->removeListener(this);
	};

	void paint(juce::Graphics& g) override
	{
		//Parameters
		auto bounds = getLocalBounds();
		const int subDivisionLinesHorizontal{ 3 };
		const int subDivisionLinesVertical{ 7 };

		//Draw background
		g.setColour(EditorColours::darkblue);
		g.fillRect(bounds);
		bounds.reduce(borderStroke, borderStroke);

		//Draw lines horizontal
		auto r{ juce::Rectangle<int>() };
		r.setHeight(lineStroke);
		r.setWidth(bounds.getWidth());
		for(int i{ 0 }; i < subDivisionLinesHorizontal; i++)
		{
			if(i % 2)
				g.setColour(EditorColours::black);
			else
				g.setColour(EditorColours::grey);

			r.setCentre(bounds.getCentreX(), (i + 1) * bounds.getHeight() / (subDivisionLinesHorizontal + 1) + lineStroke * 2.f);
			g.fillRect(r);
		}

		//Draw lines vertical
		r.setHeight(bounds.getHeight());
		r.setWidth(lineStroke);
		for(int i{ 0 }; i < subDivisionLinesVertical; i++)
		{
			if(i % 2)
				g.setColour(EditorColours::black);
			else
				g.setColour(EditorColours::grey);

			r.setCentre((i + 1) * bounds.getWidth() / (subDivisionLinesVertical + 1) + lineStroke * 2.f, bounds.getCentreY());
			g.fillRect(r);
		}

		//Draw input graph
		const auto levelLeft{ juce::Decibels::decibelsToGain(valueSupplierLeft()) };
		const auto levelRight{ juce::Decibels::decibelsToGain(valueSupplierRight()) };
		const float peak{ juce::jmax(levelLeft, levelRight) };

		if(peak > juce::Decibels::decibelsToGain(-60.f))
		{
			const float driveInput{ juce::jmin(peak * (1 - mixValue) + peak * driveInGain * mixValue,2.f) };

			if(driveInput != 0)
			{
				const int inputCurveSampling{ int(functionCurveSampling / 4 * driveInput * 2) };
				bounds.reduce(0, lineStroke / 2.f);
				bounds.reduce(bounds.getWidth() * (1 - juce::jmap(driveInput, 0.f, 2.f, 0.f, 1.f)) / 2.f, 0);
				inputCurve.clear();
				for(int i{ 0 }; i <= inputCurveSampling; i++)
				{
					float output = graphFunction(juce::jmap(float(i), 0.f, float(inputCurveSampling), -driveInput, driveInput));
					float x = bounds.getX() + bounds.getWidth() / float(inputCurveSampling) * i;
					float y = bounds.getY() + bounds.getHeight() * juce::jmap(output, -1.f, 1.f, 1.f, 0.f);

					if(i == 0)
					{
						inputCurve.startNewSubPath(x, y);
					}
					else
					{
						inputCurve.lineTo(x, y);
					}
				}
				g.setColour(EditorColours::white);
				g.strokePath(inputCurve, juce::PathStrokeType(lineStroke * 3.f, juce::PathStrokeType::beveled, juce::PathStrokeType::rounded));
			}
		}

		//Draw graph path
		g.setColour(EditorColours::lightblue);
		g.strokePath(functionCurve, juce::PathStrokeType(lineStroke, juce::PathStrokeType::beveled, juce::PathStrokeType::rounded));

		//Draw border
		g.setColour(EditorColours::black);
		g.drawRect(getLocalBounds(), int(borderStroke));
	}

	void parameterValueChanged(int parameterIndex, float newValue) override
	{
		parametersChanged.set(true);
	}

	void parameterGestureChanged(int paramterIndex, bool gestureIsStarting) override {}

	void timerCallback() override
	{
		if(parametersChanged.compareAndSetBool(false, true))
		{
			updateGraph();
		}

		repaint();
	}

	void updateGraph()
	{
		//Updating params
		Parameters::ChainSettings settings = Parameters::getChainSettings(p_apvts);
		driveInGain = juce::Decibels::decibelsToGain(settings.driveInDecibels);
		mixValue = settings.mix;
		float mix{ settings.mix };
		float hardness{ settings.hardness };
		float outputGain{ juce::Decibels::decibelsToGain(settings.outputGainInDecibels) };

		//Setting correct function
		switch(settings.clippingType)
		{
			case Parameters::ClippingType::SoftClipping:
				graphFunction = [hardness, mix, outputGain](float x)
				{
					float hardnessConstant{ hardness * 30 + 1 };
					return (2.0f / juce::MathConstants<float>::pi * atan(x * hardnessConstant) * mix + x * (1 - mix)) * outputGain;
				};
				break;
			case Parameters::ClippingType::HardClipping:
				graphFunction = [hardness, mix, outputGain](float x)
				{
					float hardnessConstant{ std::clamp(1.0f - hardness, 0.001f, 1.0f) }; //prevent from going to 0
					return (juce::jlimit(float(-hardnessConstant), float(hardnessConstant), x) * 1 / hardnessConstant * mix + x * (1 - mix)) * outputGain;
				};
				break;
			case Parameters::ClippingType::SineFold:
				graphFunction = [hardness, mix, outputGain](float x)
				{
					float maxFolds{ 20.f }, minFolds{ 0.5f };
					float hardnessConstant{ (hardness * (maxFolds - minFolds) + minFolds) * (juce::MathConstants<float>::pi / 2.f) };

					return (sin(x * hardnessConstant) * mix + x * (1 - mix)) * outputGain;
				};
				break;
			//case Parameters::ClippingType::SawFold:
			//	graphFunction = [hardness, mix, outputGain](float x)
			//	{
			//		float maxFolds{ 20.f }, minFolds{ 0.5f };
			//		float hardnessConstant{ hardness * (maxFolds - minFolds) + minFolds };
			//
			//
			//		return (fmod(x * hardnessConstant, 1.f) * mix + x * (1 - mix)) * outputGain;
			//	};
			default:
				graphFunction = [](float x)
				{
					return x;
				};
				break;
		}

		//Curve path generation
		auto bounds = getLocalBounds();
		bounds.reduce(borderStroke, borderStroke);
		bounds.reduce(0, lineStroke / 2.f);

		functionCurve.clear();
		for(int i{ 0 }; i <= functionCurveSampling; i++)
		{
			float output = graphFunction(juce::jmap(float(i), 0.f, float(functionCurveSampling), -2.f, 2.f));
			float x = bounds.getX() + bounds.getWidth() / float(functionCurveSampling) * i;
			float y = bounds.getY() + bounds.getHeight() * juce::jmap(output, -1.f, 1.f, 1.f, 0.f);

			if(i == 0)
			{
				functionCurve.startNewSubPath(x, y);
			}
			else
			{
				functionCurve.lineTo(x, y);
			}
		}
	}

private:
	juce::AudioProcessorValueTreeState* p_apvts;
	std::function<float()> valueSupplierLeft;
	std::function<float()> valueSupplierRight;
	juce::Atomic<bool> parametersChanged{ true };


	std::function<float(float)> graphFunction;
	juce::Path functionCurve;
	juce::Path inputCurve;
	float driveInGain{ 0.0f };
	float mixValue{ 0.0f };
	const float borderStroke{ 4.0f };
	const float lineStroke{ 2.0f };
	const int functionCurveSampling{ 400 };
};
#endif