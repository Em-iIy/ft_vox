/*
Created by: Emily (Em_iIy) Winnink
Created on: 26/01/2026
*/

#pragma once

#include "glu/gl-utils.hpp"

struct SkyTimeSettings;

struct GradientDTO {
	mlm::vec4	sunriseCol;
	mlm::vec4	dayLowCol;
	mlm::vec4	dayHighCol;
	mlm::vec4	sunsetCol;
	mlm::vec4	nightLowCol;
	mlm::vec4	nightHighCol;
};

class SkyGradient
{
	public:
		SkyGradient();
		SkyGradient(const GradientDTO &dto, const SkyTimeSettings &timeSettings);
		~SkyGradient();

		void		load(const GradientDTO &dto, const SkyTimeSettings &timeSettings);

		mlm::vec4	sampleAt(const float t) const;

	private:
		std::vector<std::pair<float, mlm::vec4>>	_stops;
};
