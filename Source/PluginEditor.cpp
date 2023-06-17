/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDistortionAudioProcessorEditor::SimpleDistortionAudioProcessorEditor(SimpleDistortionAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.

	driveKnob = std::make_unique <KnobStyle>();
	hardnessKnob = std::make_unique <KnobStyle>();
	mixKnob = std::make_unique <KnobStyle>();
	outputGainKnob = std::make_unique <KnobStyle>();

	for(auto* comp : getComps())
	{
		addAndMakeVisible(comp);
	}

	setSize(1350, 470);
}

SimpleDistortionAudioProcessorEditor::~SimpleDistortionAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDistortionAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}

void SimpleDistortionAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..

	auto bounds = getLocalBounds();
	int borderSize = int(bounds.getHeight() * 0.05f);
	bounds.reduce(borderSize, borderSize);

	auto graphArea = bounds.removeFromRight(bounds.getHeight() * 2);

	auto leftMeterArea = bounds.removeFromLeft(int(bounds.getWidth() * 0.1f));
	auto rightMeterArea = bounds.removeFromRight(leftMeterArea.getWidth());

	auto titleArea = bounds.removeFromTop(int(bounds.getHeight() * 0.1f));
	auto dropDownArea = bounds.removeFromBottom(int(bounds.getHeight() * 0.05f));

	auto leftKnobArea = bounds.removeFromLeft(int(bounds.getWidth() * 0.25f));
	auto rightKnobArea = bounds.removeFromRight(leftKnobArea.getWidth());


	hardnessKnob.get()->setBounds(leftKnobArea.removeFromTop(int(leftKnobArea.getHeight() * 0.5f)));
	mixKnob.get()->setBounds(leftKnobArea);
	
	outputGainKnob.get()->setBounds(rightKnobArea.removeFromTop(int(rightKnobArea.getHeight() * 0.5f)));
	
	driveKnob.get()->setBounds(bounds);
}

std::vector<juce::Component*> SimpleDistortionAudioProcessorEditor::getComps()
{
	return
	{
		driveKnob.get(),
		hardnessKnob.get(),
		mixKnob.get(),
		outputGainKnob.get()
	};
}

