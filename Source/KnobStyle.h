#ifndef KNOBSTYLE_H
#define KNOBSTYLE_H

#pragma once
#include <JuceHeader.h>


struct KnobStyle : juce::Slider
{
public:
	KnobStyle() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
	{
	}

private:

};

#endif