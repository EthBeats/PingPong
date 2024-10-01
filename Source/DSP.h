/*
  ==============================================================================

    DSP.h
    Created: 10 Jul 2024 2:19:04pm
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

inline void panningEqualPower(float panning, float& left, float& right)
{
	float x = 0.7853981633974483f * (panning + 1.f);
	left = std::cos(x);
	right = std::sin(x);
}
