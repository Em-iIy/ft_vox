/*
Created by: Emily (Em_iIy) Winnink
Created on: 03/09/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

#include "Plane.hpp"
#include "AABB.hpp"

#include <array>


class Frustum {
	public:
		void	update(const mlm::mat4 &m);
		bool	isBoxVisible(const AABB &box) const;
	
	private:
		std::array<Plane, 6>	_planes;
};