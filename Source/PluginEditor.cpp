/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
PingPongAudioProcessorComponent::PingPongAudioProcessorComponent (PingPongAudioProcessor& p)
    : audioProcessor (p), meter(p.levelL, p.levelR)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    // set up groups
    delayGroup.setText("Delay");
    delayGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    delayGroup.addAndMakeVisible(delayTimeKnob);
    delayGroup.addChildComponent(delayNoteKnob);
    addAndMakeVisible(delayGroup);
    
    tempoSyncButton.setButtonText("Sync");
    tempoSyncButton.setClickingTogglesState(true);
    tempoSyncButton.setBounds(0, 0, 70, 27);
    tempoSyncButton.setLookAndFeel(ButtonLookAndFeel::get());
    delayGroup.addAndMakeVisible(tempoSyncButton);
    
    feedbackGroup.setText("Feedback");
    feedbackGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    feedbackGroup.addAndMakeVisible(feedbackKnob);
    feedbackGroup.addAndMakeVisible(stereoKnob);
    feedbackGroup.addAndMakeVisible(lowCutKnob);
    feedbackGroup.addAndMakeVisible(highCutKnob);
    addAndMakeVisible(feedbackGroup);
    
    outputGroup.setText("Output");
    outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(gainKnob);
    outputGroup.addAndMakeVisible(mixKnob);
    outputGroup.addAndMakeVisible(meter);
    addAndMakeVisible(outputGroup);
    
    // set main look-and-feel
    setLookAndFeel(&mainLNF);
    
    juce::Image bypassIcon = juce::ImageCache::getFromMemory(BinaryData::Bypass_png,
															 BinaryData::Bypass_pngSize);
	bypassButton.setClickingTogglesState(true);
	bypassButton.setBounds(0, 0, 20, 20);
	bypassButton.setImages(false, true, true, bypassIcon, 1.f, juce::Colours::white,
											  bypassIcon, 1.f, juce::Colours::white,
											  bypassIcon, 1.f, juce::Colours::grey, 0.f);
	addAndMakeVisible(bypassButton);
    
    updateDelayKnobs(audioProcessor.params.tempoSyncParam->get());
    audioProcessor.params.tempoSyncParam->addListener(this);
}

PingPongAudioProcessorComponent::~PingPongAudioProcessorComponent()
{
	audioProcessor.params.tempoSyncParam->removeListener(this);
	setLookAndFeel(nullptr);
}

//==============================================================================
void PingPongAudioProcessorComponent::parameterValueChanged(int, float value)
{
	DBG("parameter changed: " << value);
	
	if (juce::MessageManager::getInstance()->isThisTheMessageThread()) {
		updateDelayKnobs(value != 0.f);
	} else {
		juce::MessageManager::callAsync([this, value]
		{
			updateDelayKnobs(value != 0.f);
		});
	}
}

void PingPongAudioProcessorComponent::updateDelayKnobs(bool tempoSyncActive)
{
	delayTimeKnob.setVisible(!tempoSyncActive);
	delayNoteKnob.setVisible(tempoSyncActive);
}

void PingPongAudioProcessorComponent::paint (juce::Graphics& g)
{
	// fill background
	//g.fillAll(Colors::background);
	g.setGradientFill(juce::ColourGradient::vertical(Colors::backgroundTop, 40, Colors::backgroundBottom, getHeight()));
	g.fillRect(getLocalBounds());
	
    // add noise to the background
//    juce::Image noise = juce::ImageCache::getFromMemory(BinaryData::Noise_png,
//														BinaryData::Noise_pngSize);
//	// scale by 0.5 for Retina displays but makes weird lines when resizing...
//	// setting to 1.0 fixes it but reduces quality
//	juce::FillType fillType = juce::FillType(noise, juce::AffineTransform::scale(1.f));
//	g.setFillType(fillType);
//	g.fillRect(getLocalBounds());
    
    // add background header
//    juce::Rectangle<int> rect = getLocalBounds().withHeight(40);
//    g.setColour(Colors::header);
//    g.fillRect(rect);
    
    // get header image from binary data
    juce::Image image = juce::ImageCache::getFromMemory(BinaryData::Logo_png,
														BinaryData::Logo_pngSize);
	// draw header image
	int destWidth = image.getWidth() / 2;
	int destHeight = image.getHeight() / 2;
	g.drawImage(image, getWidth() / 2 - destWidth / 2, 0, destWidth, destHeight,
				0, 0, image.getWidth(), image.getHeight());
	
//	juce::Image logo = juce::ImageCache::getFromMemory(BinaryData::EB_png,
//													   BinaryData::EB_pngSize);
//	int dWidth = logo.getWidth() / 2;
//	int dHeight = logo.getHeight() / 2;
//	g.drawImage(logo, 5, -3, dWidth, dHeight, 0, 0, logo.getWidth(), logo.getHeight());
}

void PingPongAudioProcessorComponent::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    int y = 50;
    int height = getHeight() - 60;
    
    // Position the groups
    delayGroup.setBounds(10, y, 110, height);
    
    outputGroup.setBounds(getWidth() - 160, y, 150, height);
    
    feedbackGroup.setBounds(delayGroup.getRight() + 10, y, outputGroup.getX() - delayGroup.getRight() - 20, height);
    
    // position the knobs within the groups
    delayTimeKnob.setTopLeftPosition(20, 20);
    tempoSyncButton.setTopLeftPosition(20, delayTimeKnob.getBottom() + 10);
    delayNoteKnob.setTopLeftPosition(delayTimeKnob.getX(), delayTimeKnob.getY());
    mixKnob.setTopLeftPosition(20, 20);
    gainKnob.setTopLeftPosition(mixKnob.getX(), mixKnob.getBottom() + 10);
    feedbackKnob.setTopLeftPosition(20, 20);
    stereoKnob.setTopLeftPosition(feedbackKnob.getRight() + 20, 20);
    lowCutKnob.setTopLeftPosition(feedbackKnob.getX(), feedbackKnob.getBottom() + 10);
    highCutKnob.setTopLeftPosition(lowCutKnob.getRight() + 20, lowCutKnob.getY());
    
    // position the meter
    meter.setBounds(outputGroup.getWidth() - 45, 30, 30, gainKnob.getBottom() - 30);
    
    // position bypass button
    bypassButton.setTopLeftPosition(getRight() - bypassButton.getWidth() - 10, 10);
}

//==============================================================================
ResizeablePingPongAudioProcessorEditor::ResizeablePingPongAudioProcessorEditor(PingPongAudioProcessor& p)
	: juce::AudioProcessorEditor(p), pingPongComponent(p)
{
	// set up options file for saving window size
	juce::PropertiesFile::Options options;
    options.applicationName = ProjectInfo::projectName;
    options.folderName = ProjectInfo::companyName;
    options.commonToAllUsers = true;
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    
    applicationProperties.setStorageParameters(options);
    
    // get scale value and set to 1.0 if no value is found
    double scale = 1.0;
    if (juce::PropertiesFile* properties = applicationProperties.getCommonSettings(true)) {
		scale = properties->getDoubleValue("ScaleFactor", 1.0);
    }
    
	addAndMakeVisible(pingPongComponent);
	
	// set up window and resize limits
	setSize(int(pluginWidth * scale), int(pluginHeight * scale));
    setResizable(true, true);
    setResizeLimits(pluginWidth, pluginHeight, pluginWidth * 2, pluginHeight * 2);
    getConstrainer()->setFixedAspectRatio(pluginWidth / float(pluginHeight));
}

ResizeablePingPongAudioProcessorEditor::~ResizeablePingPongAudioProcessorEditor()
{
}

//==============================================================================
void ResizeablePingPongAudioProcessorEditor::resized()
{
	// set scale factor based on window width
	const float scaleFactor = getWidth() / float(pluginWidth);
	
	// set scale if options file exists
	if (juce::PropertiesFile* properties = applicationProperties.getCommonSettings(true)) {
		properties->setValue("ScaleFactor", scaleFactor);
	}
	
	// transform window and set its bounds
	pingPongComponent.setTransform(juce::AffineTransform::scale(scaleFactor));
	pingPongComponent.setBounds(0, 0, pluginWidth, pluginHeight);
	
	// debug statements for printing scale factor and window size
	DBG("Scale Factor: " << scaleFactor << '\n');
	DBG("Window Bounds: " << getBounds().toString() << '\n');
}
