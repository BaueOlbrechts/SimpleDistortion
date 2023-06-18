#include "DropDownStyle.h"

void DropDownLookAndFeel::drawComboBox(juce::Graphics& g,
	int 	width,
	int 	height,
	bool 	isButtonDown,
	int 	buttonX,
	int 	buttonY,
	int 	buttonW,
	int 	buttonH,
	juce::ComboBox& comboBox)
{
	comboBox.setJustificationType(juce::Justification::centredLeft);

	auto bounds = juce::Rectangle<float>(float(buttonX), float(buttonY), float(buttonW), float(buttonH));
	g.setColour(EditorColours::darkblue);
	g.fillRect(bounds);
	g.setColour(EditorColours::black);
	g.drawRect(bounds, 2.f);

}

/*
void DropDownLookAndFeel::drawPopupMenuItem(juce::Graphics& g,
	const juce::Rectangle< int >& area,
	bool 	isSeparator,
	bool 	isActive,
	bool 	isHighlighted,
	bool 	isTicked,
	bool 	hasSubMenu,
	const juce::String& text,
	const juce::String& shortcutKeyText,
	const juce::Drawable* icon,
	const juce::Colour* textColour)
{
}
*/

DropDownStyle::~DropDownStyle()
{
	setLookAndFeel(nullptr);
}

void DropDownStyle::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds();

	getLookAndFeel().drawComboBox(g,
		bounds.getWidth(),
		bounds.getHeight(),
		false,
		bounds.getX(),
		bounds.getY(),
		bounds.getWidth(),
		bounds.getHeight(),
		*this);

}

juce::String DropDownStyle::getNameString() const
{
	return displayName;
}