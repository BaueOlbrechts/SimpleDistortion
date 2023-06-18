/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "KnobStyle.h"
#include "LevelMeterStyle.h"

//==============================================================================
/**
*/
class SimpleDistortionAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	SimpleDistortionAudioProcessorEditor(SimpleDistortionAudioProcessor&);
	~SimpleDistortionAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	SimpleDistortionAudioProcessor& audioProcessor;

	//std::unique_ptr <KnobStyle> 
	KnobStyle
		driveKnob,
		hardnessKnob,
		mixKnob,
		outputGainKnob;

	using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	Attachment driveKnobAttachment,
		hardnessKnobAttachment,
		mixKnobAttachment,
		outputGainKnobAttachment;

	LevelMeter lmInput, lmOutput;

	std::vector<juce::Component*> getComps();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDistortionAudioProcessorEditor)
};
