/*
Created by: Emily (Em_iIy) Winnink
Created on: 03/09/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

class AABB {
	public:
		mlm::vec3	_min;
		mlm::vec3	_max;
	
		AABB();
		AABB(const mlm::vec3 &min, const mlm::vec3 &max);

		mlm::vec3	getPositiveVertex(const mlm::vec3 &normal) const;
};