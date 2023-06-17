#ifndef KNOBSTYLE_H
#define KNOBSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"

struct KnobLookAndFeel : juce::LookAndFeel_V4
{
public:
	void drawRotarySlider(juce::Graphics& g,
		int 	x,
		int 	y,
		int 	width,
		int 	height,
		float 	sliderPosProportional,
		float 	rotaryStartAngle,
		float 	rotaryEndAngle,
		juce::Slider& slider) override;
};


struct KnobStyle : juce::Slider
{
public:
	KnobStyle(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
		juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox),
		param(&rap),
		suffix(unitSuffix)
	{
		setLookAndFeel(&lnf);
	}

	~KnobStyle();
	void paint(juce::Graphics& g) override;
	juce::Rectangle<int> getSliderBounds() const;
	int getTextHeight() const;
	juce::String getDisplayString() const;

private:
	KnobLookAndFeel lnf;
	juce::RangedAudioParameter* param;
	juce::String suffix;
};

#endif