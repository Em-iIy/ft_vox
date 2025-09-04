/*
Created by: Emily (Em_iIy) Winnink
Created on: 03/09/2025
*/

#include "Plane.hpp"

Plane::Plane(): _normal(0.0f), _d(0.0f)
{}

Plane::Plane(const mlm::vec3 &n, float d): _normal(mlm::normalize(n)), _d(d)
{}

Plane::Plane(const mlm::vec4 &eq)
{
	_normal = mlm::vec3(eq);
	float length = mlm::abs(_normal);
	_normal /= length;
	_d = eq.w / length;
}

void		Plane::normalize()
{
	float	length = mlm::abs(_normal);
	if (length > 0.0f)
	{
		_normal /= length;
		_d /= length;
	}
}

float		Plane::distance(const mlm::vec3 &p) const
{
	return (mlm::dot(_normal, p) + _d);
}

const mlm::vec3	&Plane::getNormal() const
{
	return (_normal);
}
