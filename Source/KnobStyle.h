#ifndef KNOBSTYLE_H
#define KNOBSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"


struct KnobLookAndFeel : juce::LookAndFeel_V4
{
	void drawRotarySlider(juce::Graphics& g,
		int 	x,
		int 	y,
		int 	width,
		int 	height,
		float 	sliderPosProportional,
		float 	rotaryStartAngle,
		float 	rotaryEndAngle,
		juce::Slider& slider) override
	{
		jassert(rotaryStartAngle < rotaryEndAngle);

		auto bounds = juce::Rectangle<float>(float(x), float(y), float(width), float(height));
		g.setColour(EditorColours::lightblue);
		g.fillEllipse(bounds);

		g.setColour(EditorColours::black);
		g.drawEllipse(bounds, 1.f);


		//Pointer line
		auto center = bounds.getCentre();
		juce::Path p;
		juce::Rectangle<float> r;
		r.setLeft(center.getX() - 2);
		r.setRight(center.getX() + 2);
		r.setTop(bounds.getY());
		r.setBottom(center.getY());
		p.addRectangle(r);

		auto sliderAngleRadians = juce::jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
		p.applyTransform(juce::AffineTransform().rotated(sliderAngleRadians, center.getX(), center.getY()));
		g.fillPath(p);
	}

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

	~KnobStyle()
	{
		setLookAndFeel(nullptr);
	}

	void paint(juce::Graphics& g) override
	{
		auto startAngle = juce::degreesToRadians(180.f + 45.f);
		auto endAngle = juce::degreesToRadians(180.f - 45.f) + juce::MathConstants<float>::twoPi;

		auto range = getRange();

		auto sliderBounds = getSliderBounds();

		//Test to see bounds
		g.setColour(juce::Colours::red);
		g.drawRect(getLocalBounds());
		g.setColour(juce::Colours::yellow);
		g.drawRect(sliderBounds);

		//Get paint from LookAndFeel
		getLookAndFeel().drawRotarySlider(g,
			sliderBounds.getX(),
			sliderBounds.getY(),
			sliderBounds.getWidth(),
			sliderBounds.getHeight(),
			float(juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0)),
			startAngle,
			endAngle, 
			*this);
	}

	juce::Rectangle<int> getSliderBounds() const
	{
		auto bounds = getLocalBounds();
		auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
		size -= getTextHeight() * 2;
		juce::Rectangle<int> r;
		r.setSize(size, size);
		r.setCentre(bounds.getCentreX(), 0);
		r.setY(2);

		return r;
	}

	int getTextHeight() const { return 14; };
	juce::String getDisplayString() const;

private:
	KnobLookAndFeel lnf;
	juce::RangedAudioParameter* param;
	juce::String suffix;
};

#endif