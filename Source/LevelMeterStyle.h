#ifndef LEVELMETERSTYLE_H
#define LEVELMETERSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"

class LevelMeter : public juce::Component, public juce::Timer
{
public:
	LevelMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
	{
		startTimerHz(24);
	}

	void paint(juce::Graphics& g) override
	{
		const auto level{ valueSupplier() };

		auto bounds{ getLocalBounds() };

		g.setColour(EditorColours::black);
		g.fillRect(bounds);

		g.setColour(EditorColours::lightblue);
		const auto scaledY{ juce::jmap(level,minLevel,maxLevel,0.f,static_cast<float>(getHeight())) };
		g.fillRect(bounds.removeFromBottom(scaledY));
	}

	void resized() override
	{

	}

	void timerCallback() override
	{
		repaint();
	}
private:
	std::function<float()> valueSupplier;
	float minLevel = -60.f;
	float maxLevel = 6.f;
};

#endif