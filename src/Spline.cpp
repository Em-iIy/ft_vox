/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/08/2025
*/

#include "Spline.hpp"

#include <algorithm>

Spline::Spline()
{
}

Spline::Spline(const std::vector<mlm::vec2> &points)
{
	setPoints(points);
}

Spline::Spline(const Spline &src)
{
	setPoints(src._points);
}

Spline	&Spline::operator=(const Spline &src)
{
	setPoints(src._points);
	return (*this);
}

Spline::~Spline()
{
}

void	Spline::setPoints(const std::vector<mlm::vec2> &points)
{
	if (points.size() < 2)
		throw std::runtime_error("Spline must contain at least 2 points");
	
	_points = points;
	// for (const mlm::vec2 &point : _points)
	// 	std::cout << point << std::endl;
	std::sort(_points.begin(), _points.end(),
		[](const mlm::vec2 &a, const mlm::vec2 &b)
		{
			return (a.x < b.x);
		}
	);
}

float	Spline::evaluate(float t) const
{
	if (t < _points.front().x)
		return (_points.front().y);
	if (t > _points.back().x)
		return (_points.back().y);
	
	uint64_t i = 1;
	for (;i < _points.size(); ++i)
		if (_points[i].x > t)
			break ;
	
	const mlm::vec2	&p0 = (i > 1) ? _points[i - 2] : _points[0];
	const mlm::vec2	&p1 = _points[i - 1];
	const mlm::vec2	&p2 = _points[i];
	const mlm::vec2	&p3 = (i + 1 < _points.size()) ? _points[i + 1] : _points.back();

	const float	localT = (t - p1.x) / (p2.x - p1.x);

	return (catmullRom(p0.y, p1.y, p2.y, p3.y, localT));
}

bool	Spline::isReady()
{
	if (_points.size() < 2)
		return (false);
	return (true);
}

// https://www.mvps.org/directx/articles/catmull/
float	Spline::catmullRom(const float y0, const float y1, const float y2, const float y3, const float t) const
{
	const float	t2 = t * t;
	const float	t3 = t2 * t;
	return (0.5f * (
		(2.0f * y1) +
		(-y0 + y2) * t +
		(2.0f * y0 - 5.0f * y1 + 4.0f * y2 - y3) * t2 +
		(-y0 + 3.0f * y1 - 3.0f * y2 + y3) * t3
	));
}
