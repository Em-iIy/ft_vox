/*
Created by: Emily (Em_iIy) Winnink
Created on: 26/01/2026
*/

#pragma once

#include "SkyGradient.hpp"

class Sky
{
public:
	Sky();
	~Sky();
private:
	// Holds the colors for all times of the day for it's specific stop
	SkyGradient _gradientStop0;
	SkyGradient _gradientStop1;
	SkyGradient _gradientStop2;
	SkyGradient _gradientStop3;
};

