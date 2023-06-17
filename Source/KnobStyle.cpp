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
	//Safety
	jassert(rotaryStartAngle < rotaryEndAngle);

	//Parameters
	float outerCircleStroke{ 3.f };
	float shadowCircleScale(0.7f);
	float shadowCircleOffset(0.08f);
	float innerCircleScale{ 0.75f };
	float innerCircleStroke{ 2.f };
	float textBoxMargin{ 5.f };
	float valueFontMinSize{ 10.f };
	float valueFontScale{ 0.1f };
	float displayFontMinSize{ 12.f };
	float displayFontScale{ 0.125f };

	//Draw circles with border
	auto bounds = juce::Rectangle<float>(float(x), float(y), float(width), float(height));
	g.setColour(EditorColours::darkblue);
	g.fillEllipse(bounds);
	g.setColour(EditorColours::black);
	g.drawEllipse(bounds, outerCircleStroke);

	auto shaddowCircleBounds = juce::Rectangle<float>(bounds.getCentreX() - float(width) * (shadowCircleScale / 2 + shadowCircleOffset), bounds.getCentreY() - float(height) * (shadowCircleScale / 2 - shadowCircleOffset), float(width) * shadowCircleScale, float(height) * shadowCircleScale);
	g.setColour(EditorColours::transparentBlack);
	g.fillEllipse(shaddowCircleBounds);

	auto innerCircleBounds = juce::Rectangle<float>(bounds.getCentreX() - float(width) * innerCircleScale / 2, bounds.getCentreY() - float(height) * innerCircleScale / 2, float(width) * innerCircleScale, float(height) * innerCircleScale);
	g.setColour(EditorColours::lightblue);
	g.fillEllipse(innerCircleBounds);
	g.setColour(EditorColours::black);
	g.drawEllipse(innerCircleBounds, innerCircleStroke);


	if(auto* knobStyle = dynamic_cast<KnobStyle*>(&slider))
	{
		//Pointer line
		auto center = bounds.getCentre();
		juce::Path p;
		juce::Rectangle<float> r;

		r.setLeft(center.getX() - innerCircleStroke / 2.f);
		r.setRight(center.getX() + innerCircleStroke / 2.f);
		r.setTop(innerCircleBounds.getY());
		r.setBottom(center.getY());
		p.addRoundedRectangle(r, innerCircleStroke / 2.f);

		auto sliderAngleRadians = juce::jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
		p.applyTransform(juce::AffineTransform().rotated(sliderAngleRadians, center.getX(), center.getY()));
		g.fillPath(p);


		//Draw value text box
		g.setFont(valueFontMinSize + bounds.getWidth() * valueFontScale);
		g.getCurrentFont().setBold(true);
		auto text = knobStyle->getValueString();

		r.setSize(bounds.getWidth() * innerCircleScale, (valueFontMinSize + bounds.getWidth() * valueFontScale) * 1.2f);
		r.setCentre(bounds.getCentreX(), bounds.getY() - r.getHeight() / 2.f - textBoxMargin);

		g.setColour(EditorColours::lightblue);
		g.fillRect(r);
		g.setColour(EditorColours::black);
		g.drawRect(r);

		g.setColour(EditorColours::black);
		g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);

		//Draw display name
		g.setFont(displayFontMinSize + bounds.getWidth() * displayFontScale);
		g.getCurrentFont().setBold(true);
		r.setSize(bounds.getWidth(), (displayFontMinSize + bounds.getWidth() * displayFontScale) * 2.4f);
		r.setCentre(bounds.getCentreX(), bounds.getBottom() + r.getHeight() / 2.f + textBoxMargin);

		g.setColour(EditorColours::white);
		g.drawFittedText(knobStyle->getNameString(), r.toNearestInt(), juce::Justification::centredTop, 2, 0.8f);
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
	//Create params
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
	juce::Rectangle<int> r;
	r.setSize(size, size);
	r.setCentre(bounds.getCentreX(), bounds.getCentreY());
	return r;
}

juce::String KnobStyle::getValueString() const
{
	if(auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
		return choiceParam->getCurrentChoiceName();

	juce::String str;

	if(auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
	{
		float val = getValue();
		if(suffix.isNotEmpty())
		{
			str = juce::String(val, 1);
			str << " ";
			str << suffix;
		}
		else
		{
			str = juce::String(val, 2);
		}
	}
	else
	{
		jassertfalse;
	}
	return str;
}

juce::String KnobStyle::getNameString() const
{
	return displayName;
}
