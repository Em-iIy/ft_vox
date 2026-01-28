/*
Created by: Emily (Em_iIy) Winnink
Created on: 26/01/2026
*/

#pragma once

#include "SkyGradient.hpp"

struct	SkyTimeSettings
{
	// Length of differnt times of day in seconds
	float	sunriseLen = {};
	float	dayLen = {};
	float	sunsetLen = {};
	float	nightLen = {};
};

struct	SkyDTO
{
	SkyTimeSettings	timeSettings;
	GradientDTO		stop0;
	GradientDTO		stop1;
	GradientDTO		stop2;
	GradientDTO		stop3;
};

class	Sky
{
	public:
		Sky();
		Sky(const SkyDTO &dto);
		~Sky();

		void	load(const SkyDTO &dto);
		void	update(const float deltaTime);
		void	togglePause();
		float	getTime() const;
		float	getTimePercent() const;

		// Holds the colors for all times of the day for it's specific stop
		SkyGradient		_gradientStop0;
		SkyGradient		_gradientStop1;
		SkyGradient		_gradientStop2;
		SkyGradient		_gradientStop3;

	private:
		SkyTimeSettings	_timeSettings;

		float			_time = {};
		bool			_paused = false;

		float			_getTotalTime() const;
};
