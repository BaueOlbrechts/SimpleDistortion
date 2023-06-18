#ifndef LEVELMETERSTYLE_H
#define LEVELMETERSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"

class LevelMeter : public juce::Component, public juce::Timer
{
public:
	LevelMeter(std::function<float()>&& valueFunctionLeft, std::function<float()>&& valueFunctionRight) :
		valueSupplierLeft(std::move(valueFunctionLeft)),
		valueSupplierRight(std::move(valueFunctionRight))
	{
		startTimerHz(24);
	}

	void paint(juce::Graphics& g) override
	{
		const auto levelLeft{ valueSupplierLeft() };
		const auto levelRight{ valueSupplierRight() };
		auto bounds{ getLocalBounds() };

		float borderScale{ 0.12f };
		float dividerScale{ 0.1f };
		float backgroundDarkenValue{ 0.5f };
		float lightAreaScale{ 2.f };


		//Background draw
		g.setColour(EditorColours::black);
		g.fillRect(bounds);
		bounds.reduce(bounds.getWidth() * borderScale, bounds.getWidth() * borderScale);

		//Create middle line
		auto center = bounds.getCentre();
		juce::Rectangle<float> rMiddle;
		rMiddle.setLeft(center.getX() - bounds.getWidth() * dividerScale / 2.f);
		rMiddle.setRight(center.getX() + bounds.getWidth() * dividerScale / 2.f);
		rMiddle.setTop(bounds.getY());
		rMiddle.setBottom(bounds.getBottom());

		//Meters draw
		paintMeterStlye2(g, bounds.removeFromLeft(bounds.getWidth() * 0.5f), levelLeft, backgroundDarkenValue, lightAreaScale);
		paintMeterStlye2(g, bounds, levelRight, backgroundDarkenValue, lightAreaScale);


		//Draw middle line
		g.setColour(EditorColours::black);
		g.fillRect(rMiddle);
	}

	void resized() override
	{

	}

	void timerCallback() override
	{
		repaint();
	}
private:
	std::function<float()> valueSupplierLeft;
	std::function<float()> valueSupplierRight;
	float minLevel = -60.f;
	float maxLevel = 6.f;

	void paintMeterStlye1(juce::Graphics& g, juce::Rectangle<int> bounds, float level, float backgroundDarkenValue, float lightAreaScale)
	{
		auto boundsBackup{ bounds };
		//Draw background
		g.setColour(EditorColours::red.darker(backgroundDarkenValue));
		g.fillRect(bounds.removeFromTop(bounds.getHeight() * (maxLevel / (maxLevel - minLevel))));
		g.setColour(EditorColours::lightblue.darker(backgroundDarkenValue));
		g.fillRect(bounds.removeFromTop(bounds.getHeight() * (maxLevel * lightAreaScale / (-minLevel))));
		g.setColour(EditorColours::darkblue.darker(backgroundDarkenValue));
		g.fillRect(bounds);

		bounds = boundsBackup;
		//Draw current level
		const auto scaledYLeft{ juce::jmap(level,minLevel,maxLevel,0.f,float(bounds.getHeight())) };
		if(scaledYLeft > juce::jmap(-maxLevel * lightAreaScale, minLevel, maxLevel, 0.f, float(bounds.getHeight())))
		{

			if(scaledYLeft > juce::jmap(0.f, minLevel, maxLevel, 0.f, float(bounds.getHeight())))
			{
				g.setColour(EditorColours::darkblue);
				g.fillRect(bounds.removeFromBottom(juce::jmap(-maxLevel * lightAreaScale, minLevel, maxLevel, 0.f, float(bounds.getHeight()))));
				g.setColour(EditorColours::lightblue);
				g.fillRect(bounds.removeFromBottom(juce::jmap(0.f, -maxLevel * lightAreaScale, maxLevel, 0.f, float(bounds.getHeight()))));
				g.setColour(EditorColours::red);
				g.fillRect(bounds.removeFromBottom(juce::jmap(level, 0.f, maxLevel, 0.f, float(bounds.getHeight()))));
			}
			else
			{
				g.setColour(EditorColours::darkblue);
				g.fillRect(bounds.removeFromBottom(juce::jmap(-maxLevel * lightAreaScale, minLevel, maxLevel, 0.f, float(bounds.getHeight()))));
				g.setColour(EditorColours::lightblue);
				g.fillRect(bounds.removeFromBottom(juce::jmap(level, -maxLevel * lightAreaScale, maxLevel, 0.f, float(bounds.getHeight()))));

			}

		}
		else
		{
			g.setColour(EditorColours::darkblue);
			g.fillRect(bounds.removeFromBottom(scaledYLeft));
		}
	}

	void paintMeterStlye2(juce::Graphics& g, juce::Rectangle<int> bounds, float level, float backgroundDarkenValue, float lightAreaScale)
	{
		auto boundsBackup{ bounds };
		//Draw background
		g.setColour(EditorColours::red.darker(backgroundDarkenValue));
		g.fillRect(bounds.removeFromTop(bounds.getHeight() * (maxLevel / (maxLevel - minLevel))));
		g.setColour(EditorColours::lightblue.darker(backgroundDarkenValue));
		g.fillRect(bounds.removeFromTop(bounds.getHeight() * (maxLevel * lightAreaScale / (-minLevel))));
		g.setColour(EditorColours::darkblue.darker(backgroundDarkenValue));
		g.fillRect(bounds);

		bounds = boundsBackup;

		const auto scaledYLeft{ juce::jmap(level,minLevel,maxLevel,0.f,float(bounds.getHeight())) };
		if(level >= -maxLevel * lightAreaScale)
		{

			if(level >= 0)
			{
				g.setColour(EditorColours::red);
			}
			else
			{
				g.setColour(EditorColours::lightblue);
			}

		}
		else
		{
			g.setColour(EditorColours::darkblue);
		}
		g.fillRect(bounds.removeFromBottom(scaledYLeft));
	}

};

#endif