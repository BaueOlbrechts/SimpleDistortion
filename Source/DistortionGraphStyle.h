#ifndef DISTORTIONGRAPHSTYLE_H
#define DISTORTIONGRAPHSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"
#include "Parameters.h"

class DistortionGraph : public juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
public:
	DistortionGraph(juce::AudioProcessorValueTreeState* apvts) :
		p_apvts{ apvts }
	{
		startTimerHz(24);

		p_apvts->getParameter(Parameters::ID_CLIPPINGTYPE)->addListener(this);
		p_apvts->getParameter(Parameters::ID_HARDNESS)->addListener(this);
		p_apvts->getParameter(Parameters::ID_MIX)->addListener(this);
	};

	~DistortionGraph()
	{
		p_apvts->getParameter(Parameters::ID_CLIPPINGTYPE)->removeListener(this);
		p_apvts->getParameter(Parameters::ID_HARDNESS)->removeListener(this);
		p_apvts->getParameter(Parameters::ID_MIX)->removeListener(this);
	};

	void paint(juce::Graphics& g) override
	{
		//Parameters
		auto bounds = getLocalBounds();
		float borderStroke{ 4.0f };
		float lineStroke{ 2.0f };
		int subDivisionLinesHorizontal{ 3 };
		int subDivisionLinesVertical{ 7 };

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

			r.setCentre(bounds.getCentreX(), (i + 1) * bounds.getHeight() / (subDivisionLinesHorizontal + 1));
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

			r.setCentre((i + 1) * bounds.getWidth() / (subDivisionLinesVertical + 1), bounds.getCentreY());
			g.fillRect(r);
		}


		//Curve drawing
		Parameters::ChainSettings settings = Parameters::getChainSettings(p_apvts);
		std::function<float(float)> graphFunction;
		float hardness{ settings.hardness };
		float mix{ settings.mix };

		switch(settings.clippingType)
		{
			case Parameters::ClippingType::SoftClipping:
				graphFunction = [hardness, mix](float x)
				{
					float hardnessConstant{ hardness * 10 + 1 };
					return (2.0f / 3.1415f * atan(x * hardnessConstant) * mix + x * (1 - mix));
				};
				break;
			case Parameters::ClippingType::HardClipping:
				graphFunction = [hardness, mix](float x)
				{
					float hardnessConstant{ std::clamp(1.0f - hardness, 0.05f, 1.0f) }; //prevent from going to 0
					return juce::jlimit(float(-hardnessConstant), float(hardnessConstant), x) * 1 / hardnessConstant * mix + x * (1 - mix);
				};
				break;
			case Parameters::ClippingType::SineFold:
				graphFunction = [hardness, mix](float x)
				{
					float maxFolds{ 5.f }, minFolds{ 0.5f };
					float hardnessConstant{ hardness * (maxFolds - minFolds) + minFolds };

					return sin(x * hardnessConstant) * mix + x * (1 - mix);
				};
				break;
			case Parameters::ClippingType::Dummy2:
				graphFunction = [](float x)
				{
					return x;
				};
				break;
			default:
				graphFunction = [](float x)
				{
					return x;
				};
				break;
		}

		juce::Path functionCurve;
		functionCurve.startNewSubPath(bounds.getX(), bounds.getY());
		int functionSampling{ 200 };
		for(int i{ 0 }; i <= functionSampling; i++)
		{
			float output = graphFunction(juce::jmap(float(i), 0.f, float(functionSampling), -2.f, 2.f));
			float x = bounds.getX() + bounds.getWidth() / float(functionSampling) * i;
			//float y = juce::jmap(graphFunction(i), -1.f, 1.f, float(bounds.getBottom()), float(bounds.getY()));
			float y = bounds.getY() + bounds.getHeight() * juce::jmap(output, -1.f, 1.f, 1.f, 0.f);
			functionCurve.lineTo(x, y);
		}
		g.setColour(EditorColours::lightblue);
		g.strokePath(functionCurve, juce::PathStrokeType(lineStroke));

		//Draw border
		g.setColour(EditorColours::black);
		g.drawRect(getLocalBounds(), borderStroke);
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
			repaint();
		}
	}


private:
	juce::AudioProcessorValueTreeState* p_apvts;
	juce::Atomic<bool> parametersChanged{ false };

};
#endif