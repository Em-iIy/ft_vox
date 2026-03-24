/*
Created by: Emily (Em_iIy) Winnink
Created on: 08/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

#include <unordered_map>

class Perlin {
	public:
		void									setSeed(uint64_t seed);
		float									getValue(float x, float y);
		float									getValue(float x, float y, float z);

	private:
		uint64_t								_seed = 1;
		std::unordered_map<uint64_t, mlm::vec2>	_2dGradients;
		std::unordered_map<uint64_t, mlm::vec3>	_3dGradients;

		float									_smoothStep(float t) const;
		mlm::vec2								_gradient(int x, int y);
		mlm::vec3								_gradient(int x, int y, int z);
		uint64_t								_hash(int x, int y) const;
		uint64_t								_hash(int x, int y, int z) const;
};
