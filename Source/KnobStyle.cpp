#pragma once
#include "KnobStyle.h"

void KnobLookAndFeel::drawRotarySlider(juce::Graphics& g,
	int 	x,
	int 	y,
	int 	width,
	int 	height,
	float 	sliderPosProportional,
	float 	rotaryStartAngle,
	float 	rotaryEndAngle,
	juce::Slider& slider)
{
	jassert(rotaryStartAngle < rotaryEndAngle);

	//Draw circles with border
	auto bounds = juce::Rectangle<float>(float(x), float(y), float(width), float(height));
	g.setColour(EditorColours::lightblue);
	g.fillEllipse(bounds);

	g.setColour(EditorColours::black);
	g.drawEllipse(bounds, 1.f);

	if(auto* knobStyle = dynamic_cast<KnobStyle*>(&slider))
	{
		//Pointer line
		auto center = bounds.getCentre();
		juce::Path p;
		juce::Rectangle<float> r;

		r.setLeft(center.getX() - 2);
		r.setRight(center.getX() + 2);
		r.setTop(bounds.getY());
		r.setBottom(center.getY() - knobStyle->getTextHeight() * 1.5f);

		p.addRoundedRectangle(r, 2.f);

		p.addRectangle(r);

		auto sliderAngleRadians = juce::jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
		p.applyTransform(juce::AffineTransform().rotated(sliderAngleRadians, center.getX(), center.getY()));
		g.fillPath(p);


		//Draw text box
		g.setFont(knobStyle->getTextHeight());
		auto text = knobStyle->getDisplayString();
		auto stringWidth = g.getCurrentFont().getStringWidth(text);

		r.setSize(stringWidth + 4, knobStyle->getTextHeight() + 2);
		r.setCentre(bounds.getCentre());

		g.setColour(EditorColours::lightblue);
		g.fillRect(r);

		g.setColour(EditorColours::black);
		g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
	}
}

//KnobStyle::KnobStyle(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
//	juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox),
//	param(&rap),
//	suffix(unitSuffix)
//{
//	setLookAndFeel(&lnf);
//}

KnobStyle::~KnobStyle()
{
	setLookAndFeel(nullptr);
}

void KnobStyle::paint(juce::Graphics& g)
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

juce::Rectangle<int> KnobStyle::getSliderBounds() const
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

int KnobStyle::getTextHeight() const { return 14; };

juce::String KnobStyle::getDisplayString() const
{
	if(auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
		return choiceParam->getCurrentChoiceName();

	juce::String str;

	if(auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
	{
		float val = getValue();
		str = juce::String(val, 2);

	}
	else
	{
		jassertfalse;
	}

	if(suffix.isNotEmpty())
	{
		str << " ";
		str << suffix;
	}

	return str;
}
