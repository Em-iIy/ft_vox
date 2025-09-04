/*
Created by: Emily (Em_iIy) Winnink
Created on: 03/09/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

class AABB {
	public:
		AABB();
		AABB(const mlm::vec3 &min, const mlm::vec3 &max);

		mlm::vec3	getPositiveVertex(const mlm::vec3 &normal) const;

	private:
		mlm::vec3	_min;
		mlm::vec3	_max;
};