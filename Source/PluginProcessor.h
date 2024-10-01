/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "ProtectYourEars.h"
#include "Tempo.h"
#include "DelayLine.h"
#include "Measurement.h"

//==============================================================================
/**
*/
class PingPongAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PingPongAudioProcessor();
    ~PingPongAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   // #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   // #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    /** Parameters APVTS setup **/
	juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", Parameters::createParameterLayout() };
	Parameters params;
	
	Measurement levelL, levelR;
	
	juce::AudioProcessorParameter* getBypassParameter() const override;

private:
	Tempo tempo;
	
	// linear is default second template argument
	// juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
	
	DelayLine delayLineL, delayLineR;
	
	// state variable filters
	juce::dsp::StateVariableTPTFilter<float> lowCutFilter;
	juce::dsp::StateVariableTPTFilter<float> highCutFilter;
	
	float feedbackL = 0.f;
	float feedbackR = 0.f;
	
	float lastLowCut = -1.f;
	float lastHighCut = -1.f;
	
	// crossfade for synced delay time
	float delayInSamples = 0.f;
	float targetDelay = 0.f;
	float xfade = 0.f;
	float xfadeInc = 0.f;
	
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongAudioProcessor)
};
