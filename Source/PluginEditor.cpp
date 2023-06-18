/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDistortionAudioProcessorEditor::SimpleDistortionAudioProcessorEditor(SimpleDistortionAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p),
	driveKnob(*audioProcessor.apvts.getParameter(Parameters::ID_DRIVE), Parameters::ID_DRIVE_DISPLAY, "dB"),
	hardnessKnob(*audioProcessor.apvts.getParameter(Parameters::ID_HARDNESS), Parameters::ID_HARDNESS_DISPLAY, ""),
	mixKnob(*audioProcessor.apvts.getParameter(Parameters::ID_MIX), Parameters::ID_MIX_DISPLAY, ""),
	outputGainKnob(*audioProcessor.apvts.getParameter(Parameters::ID_OUTPUT), Parameters::ID_OUTPUT_DISPLAY, "dB"),

	driveKnobAttachment(audioProcessor.apvts, Parameters::ID_DRIVE, driveKnob),
	hardnessKnobAttachment(audioProcessor.apvts, Parameters::ID_HARDNESS, hardnessKnob),
	mixKnobAttachment(audioProcessor.apvts, Parameters::ID_MIX, mixKnob),
	outputGainKnobAttachment(audioProcessor.apvts, Parameters::ID_OUTPUT, outputGainKnob),

	lmInput([&]() { return audioProcessor.getPeakValue(0, true); }, [&]() { return audioProcessor.getPeakValue(1, true); }),
	lmOutput([&]() { return audioProcessor.getPeakValue(0, false); }, [&]() { return audioProcessor.getPeakValue(1, false); }),

	clippingTypeDropDown(*audioProcessor.apvts.getParameter(Parameters::ID_CLIPPINGTYPE), Parameters::ID_CLIPPINGTYPE_DISPLAY),
	clippingTypeDropDownAttachment(audioProcessor.apvts,Parameters::ID_CLIPPINGTYPE,clippingTypeDropDown)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.

	//driveKnob = std::make_unique <KnobStyle>();
	//hardnessKnob = std::make_unique <KnobStyle>();
	//mixKnob = std::make_unique <KnobStyle>();
	//outputGainKnob = std::make_unique <KnobStyle>();

	for(auto* comp : getComps())
	{
		addAndMakeVisible(comp);
	}
	titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
	titleLabel.setText("Simple Distortion/Waveshaper",juce::dontSendNotification);
	titleLabel.setColour(juce::Label::textColourId, EditorColours::white);
	titleLabel.setJustificationType(juce::Justification::centredTop);

	setSize(1350, 470);
}

SimpleDistortionAudioProcessorEditor::~SimpleDistortionAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDistortionAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(EditorColours::black);

	auto bounds = getLocalBounds();
	int borderSize = int(bounds.getHeight() * 0.012f);
	bounds.reduce(borderSize, borderSize);
	g.setColour(EditorColours::grey);
	g.fillRect(bounds);
}

void SimpleDistortionAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..

	auto bounds = getLocalBounds();
	int marginSize = int(bounds.getHeight() * 0.035f);
	bounds.reduce(marginSize, marginSize);

	auto graphArea = bounds.removeFromRight(bounds.getHeight() * 2);

	auto leftMeterArea = bounds.removeFromLeft(int(bounds.getWidth() * 0.1f));
	auto rightMeterArea = bounds.removeFromRight(leftMeterArea.getWidth());

	auto titleArea = bounds.removeFromTop(int(bounds.getHeight() * 0.1f));

	bounds.reduce(marginSize, 0);
	auto dropDownArea = bounds.removeFromBottom(int(bounds.getHeight() * 0.07f));
	auto leftKnobArea = bounds.removeFromLeft(int(bounds.getWidth() * 0.22f));
	auto rightKnobArea = bounds.removeFromRight(leftKnobArea.getWidth());

	clippingTypeDropDown.setBounds(dropDownArea);

	hardnessKnob.setBounds(leftKnobArea.removeFromTop(int(leftKnobArea.getHeight() * 0.5f)));
	mixKnob.setBounds(leftKnobArea);
	outputGainKnob.setBounds(rightKnobArea.removeFromTop(int(rightKnobArea.getHeight() * 0.5f)));
	driveKnob.setBounds(bounds);

	lmInput.setBounds(leftMeterArea);
	lmOutput.setBounds(rightMeterArea);

	titleLabel.setBounds(titleArea);
}

std::vector<juce::Component*> SimpleDistortionAudioProcessorEditor::getComps()
{
	return
	{
		&titleLabel,

		&driveKnob,
		&hardnessKnob,
		&mixKnob,
		&outputGainKnob,

		&lmInput,
		&lmOutput,

		&clippingTypeDropDown
	};
}

