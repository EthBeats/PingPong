/*
  ==============================================================================

    DelayLine.h
    Created: 24 Jul 2024 3:32:44pm
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class DelayLine
{
public:
	DelayLine();
	~DelayLine();
	
	//==============================================================================
	void setMaximumDelayInSamples(int maxLengthInSamples);
	void reset() noexcept;
	
	void write(float input) noexcept;
	float read(float delayInSamples) const noexcept;
	
	int getBufferLength() const noexcept { return bufferLength; }
	
private:
	std::unique_ptr<float[]> buffer;
	int bufferLength = 0;
	int writeIndex = 0;
};
