#ifndef LEVELMETERSTYLE_H
#define LEVELMETERSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"

class LevelMeter : public juce::Component
{
public:
	void paint(juce::Graphics& g) override
	{
		auto bounds = getLocalBounds().toFloat();

	}

	void setLevel(const float value) { level = value; }
private:
	float level = -60.f;
};

#endif