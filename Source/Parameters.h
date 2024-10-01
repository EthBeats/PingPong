/*
  ==============================================================================

    Parameters.h
    Created: 28 Jun 2024 3:03:14pm
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP.h"

/** Parameter IDs **/
const juce::ParameterID gainParamID { "gain", 1 };
const juce::ParameterID delayTimeParamID { "delayTime", 1 };
const juce::ParameterID mixParamID { "mix", 1 };
const juce::ParameterID feedbackParamID { "feedback", 1 };
const juce::ParameterID stereoParamID { "stereo", 1 };
const juce::ParameterID lowCutParamID { "lowCut", 1 };
const juce::ParameterID highCutParamID { "highCut", 1 };
const juce::ParameterID tempoSyncParamID { "tempoSync", 1 };
const juce::ParameterID delayNoteParamID { "delayNote", 1 };
const juce::ParameterID bypassParamID { "bypass", 1 };

//==============================================================================
/**
*/
class Parameters
{
public:
	Parameters(juce::AudioProcessorValueTreeState& apvts);
	~Parameters();
	
	//==============================================================================
	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	
	void prepareToPlay(double sampleRate) noexcept;
	void reset() noexcept;
	void update() noexcept;
	void smoothen() noexcept;
	
	static constexpr float minDelayTime = 5.f;
	static constexpr float maxDelayTime = 5000.f;
	
	float gain = 0.f;
	float delayTime = 0.f;
	float mix = 1.f;
	float feedback = 0.f;
	float panL = 0.f;
	float panR = 1.f;
	float lowCut = 20.f;
	float highCut = 20000.f;
	
	int delayNote = 0;
	bool tempoSync = false;
	bool bypassed = false;
	
	juce::AudioParameterBool* tempoSyncParam;
	
	juce::AudioParameterBool* bypassParam;
	
private:
	juce::AudioParameterFloat* gainParam;
	juce::LinearSmoothedValue<float> gainSmoother;
	
	juce::AudioParameterFloat* delayTimeParam;
	
	// one-pole smoothing (analog tape effect)
	float targetDelayTime = 0.f;
	float coeff = 0.f;
	
	juce::AudioParameterFloat* mixParam;
	juce::LinearSmoothedValue<float> mixSmoother;
	
	juce::AudioParameterFloat* feedbackParam;
	juce::LinearSmoothedValue<float> feedbackSmoother;
	
	juce::AudioParameterFloat* stereoParam;
	juce::LinearSmoothedValue<float> stereoSmoother;
	
	juce::AudioParameterFloat* lowCutParam;
	juce::LinearSmoothedValue<float> lowCutSmoother;
	
	juce::AudioParameterFloat* highCutParam;
	juce::LinearSmoothedValue<float> highCutSmoother;
	
	juce::AudioParameterChoice* delayNoteParam;
	
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Parameters)
};
