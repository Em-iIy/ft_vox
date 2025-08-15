/*
Created by: Emily (Em_iIy) Winnink
Created on: 08/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

class Perlin {
	public:
		void		setSeed(uint64_t seed);
		float		getValue(float x, float y) const;
		
	private:
		uint64_t	_seed = 1;

		float		_smoothStep(float t) const;
		mlm::vec2	_gradient(int x, int y) const;
		uint64_t	_hash(int x, int y) const;
};