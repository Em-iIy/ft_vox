/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

#include <vector>

class Spline
{
	public:
		Spline(const std::vector<mlm::vec2> &points);
		~Spline();
		
		float	evaluate(float t) const;
	private:
		std::vector<mlm::vec2>	_points;

		float	catmullRom(const float y0, const float y1, const float y2, const float y3, const float t) const;
};
