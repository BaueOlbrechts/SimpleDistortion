#ifndef DROPDOWNSTYLE_H
#define DROPDOWNSTYLE_H

#pragma once
#include <JuceHeader.h>
#include "EditorColours.h"

struct DropDownLookAndFeel : juce::LookAndFeel_V4
{
public:
	void drawComboBox(juce::Graphics& g,
		int 	width,
		int 	height,
		bool 	isButtonDown,
		int 	buttonX,
		int 	buttonY,
		int 	buttonW,
		int 	buttonH,
		juce::ComboBox& comboBox) override;

	/*
	void drawPopupMenuItem(juce::Graphics& g,
		const juce::Rectangle< int >& area,
		bool 	isSeparator,
		bool 	isActive,
		bool 	isHighlighted,
		bool 	isTicked,
		bool 	hasSubMenu,
		const juce::String& text,
		const juce::String& shortcutKeyText,
		const juce::Drawable* icon,
		const juce::Colour* textColour) override;
	*/
};


struct DropDownStyle : juce::ComboBox
{
public:
	DropDownStyle(juce::RangedAudioParameter& rap, const juce::String& displName) :
		juce::ComboBox(),
		param(&rap),
		displayName(displName)
	{
		if(auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
			addItemList(choiceParam->getAllValueStrings(), 1);
		else
			jassertfalse;

		//setLookAndFeel(&lnf);

		getLookAndFeel().setColour(juce::ComboBox::ColourIds::backgroundColourId, EditorColours::darkblue);
		getLookAndFeel().setColour(juce::ComboBox::ColourIds::outlineColourId, EditorColours::black);
		getLookAndFeel().setColour(juce::ComboBox::ColourIds::arrowColourId, EditorColours::black);
		getLookAndFeel().setColour(juce::ComboBox::ColourIds::textColourId, EditorColours::white);

		getLookAndFeel().setColour(juce::PopupMenu::backgroundColourId, EditorColours::darkblue);
		getLookAndFeel().setColour(juce::PopupMenu::highlightedBackgroundColourId, EditorColours::lightblue);
		getLookAndFeel().setColour(juce::PopupMenu::highlightedTextColourId, EditorColours::white);
		getLookAndFeel().setColour(juce::PopupMenu::textColourId, EditorColours::white);

	};
	~DropDownStyle();

	void paint(juce::Graphics& g) override;
	juce::String getNameString() const;

private:
	DropDownLookAndFeel lnf;
	juce::RangedAudioParameter* param;
	juce::String displayName;

};
#endif
