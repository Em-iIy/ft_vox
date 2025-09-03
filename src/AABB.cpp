/*
Created by: Emily (Em_iIy) Winnink
Created on: 03/09/2025
*/

#include "AABB.hpp"

AABB::AABB(): _min(0.0f), _max(0.0f)
{}

AABB::AABB(const mlm::vec3 &min, const mlm::vec3 &max): _min(min), _max(max)
{}

mlm::vec3	AABB::getPositiveVertex(const mlm::vec3 &normal) const
{
	return (mlm::vec3(
		normal.x >= 0.0f ? _max.x : _min.x,
		normal.y >= 0.0f ? _max.y : _min.y,
		normal.z >= 0.0f ? _max.z : _min.z
	));
}
