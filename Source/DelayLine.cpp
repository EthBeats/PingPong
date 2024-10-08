/*
  ==============================================================================

    DelayLine.cpp
    Created: 24 Jul 2024 3:32:44pm
    Author:  Ethan Miller

  ==============================================================================
*/

#include "DelayLine.h"

//==============================================================================
DelayLine::DelayLine()
{
}

DelayLine::~DelayLine()
{
}

//==============================================================================
void DelayLine::setMaximumDelayInSamples(int maxLengthInSamples)
{
	jassert(maxLengthInSamples > 0);
	
	int paddedLength = maxLengthInSamples + 1;
	
	if (bufferLength < paddedLength) {
		bufferLength = paddedLength;
		
		buffer.reset(new float[size_t(bufferLength)]);
	}
}

void DelayLine::reset() noexcept
{
	writeIndex = bufferLength - 1;
	
	for (size_t i = 0; i < size_t(bufferLength); ++i) {
		buffer[i] = 0.f;
	}
}

void DelayLine::write(float input) noexcept
{
	jassert(bufferLength > 0);
	
	writeIndex++;
	
	if (writeIndex >= bufferLength) {
		writeIndex = 0;
	}
	
	buffer[size_t(writeIndex)] = input;
}

/* nearest neighbor rounding */
//float DelayLine::read(float delayInSamples) const noexcept
//{
//	jassert(delayInSamples >= 0.f);
//	jassert(delayInSamples <= bufferLength - 1.f);
//	
//	int readIndex = int(std::round(writeIndex - delayInSamples));
//	
//	if (readIndex < 0) {
//		readIndex += bufferLength;
//	}
//	
//	return buffer[size_t(readIndex)];
//}

/* linear interpolation */
float DelayLine::read(float delayInSamples) const noexcept
{
	jassert(delayInSamples >= 0.f);
	jassert(delayInSamples <= bufferLength - 1.f);
	
	int integerDelay = int(delayInSamples);
	
	int readIndexA = writeIndex - integerDelay;
	
	if (readIndexA < 0) {
		readIndexA += bufferLength;
	}
	
	int readIndexB = readIndexA - 1;
	
	if (readIndexB < 0) {
		readIndexB += bufferLength;
	}
	
	float sampleA = buffer[size_t(readIndexA)];
	float sampleB = buffer[size_t(readIndexB)];
	
	float fraction = delayInSamples - float(integerDelay);
	
	// same formula as one-pole filter!
	return sampleA + fraction * (sampleB - sampleA);
}

/* Hermite interpolation */
