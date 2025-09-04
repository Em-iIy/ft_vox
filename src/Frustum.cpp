/*
Created by: Emily (Em_iIy) Winnink
Created on: 03/09/2025
*/

#include "Frustum.hpp"

void	Frustum::update(const mlm::mat4 &m)
{
	mlm::vec4	row0 = mlm::row(m, 0);
	mlm::vec4	row1 = mlm::row(m, 1);
	mlm::vec4	row2 = mlm::row(m, 2);
	mlm::vec4	row3 = mlm::row(m, 3);

	_planes[0] = Plane((row3 + row0));
	_planes[1] = Plane((row3 - row0));
	_planes[2] = Plane((row3 + row1));
	_planes[3] = Plane((row3 - row1));
	_planes[4] = Plane((row3 + row2));
	_planes[5] = Plane((row3 - row2));
}

bool	Frustum::isBoxVisible(const AABB &box) const
{
	for (const Plane &plane : _planes)
	{
		mlm::vec3 p = box.getPositiveVertex(plane.getNormal());
		float dist = plane.distance(p);
		if (dist < 0)
			return (false);
	}
	return (true);
}
