/*
Created by: Emily (Em_iIy) Winnink
Created on: 03/09/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

class Plane {
	public:
		Plane();
		Plane(const mlm::vec3 &n, float d);
		Plane(const mlm::vec4 &eq);

		void		normalize();
		float		distance(const mlm::vec3 &p) const;

		const mlm::vec3	&getNormal() const;
	
	private:
		mlm::vec3	_normal;
		float		_d;
};