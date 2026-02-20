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

struct SolarBody
{
	mlm::vec4	diskColor = {};
	float		diskFactor = {};
	float		diskSize = {};
	mlm::vec4	glowColor = {};
	float		glowFactor = {};
	float		glowShaprness = {};
};


struct	SkyDTO
{
	SkyTimeSettings	timeSettings;
	SolarBody		sun;
	SolarBody		moon;
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
		void	setGradient(Shader &shader);
		void	setSolarBodies(Shader &shader);
		void	togglePause();
		float	getTime() const;
		float	getTimePercent() const;

	private:
		SkyTimeSettings	_timeSettings;

		// Holds the colors for all times of the day for it's specific stop
		SkyGradient		_gradientStop0;
		SkyGradient		_gradientStop1;
		SkyGradient		_gradientStop2;
		SkyGradient		_gradientStop3;

		// Settings for how the sun and moon looks
		SolarBody		_sun;
		SolarBody		_moon;


		float			_time = {};
		bool			_paused = false;

		GLuint			_noiseTex;
		void			_initNoise();

		float			_getTotalTime() const;
};
