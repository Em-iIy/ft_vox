/*
Created by: Emily (Em_iIy) Winnink
Created on: 08/08/2025
*/

#include "Perlin.hpp"

#include <cmath>

void	Perlin::setSeed(uint64_t seed)
{
	_seed = seed;
}

float	Perlin::getValue(float x, float y) const
{
	mlm::vec2	pos(x, y);
	int			x0 = static_cast<int>(std::floor(x));
	int			x1 = x0 + 1;
	int			y0 = static_cast<int>(std::floor(y));
	int			y1 = y0 + 1;

	mlm::vec2	g00 = _gradient(x0, y0);
	mlm::vec2	g01 = _gradient(x0, y1);
	mlm::vec2	g10 = _gradient(x1, y0);
	mlm::vec2	g11 = _gradient(x1, y1);

	float		d00 = mlm::dot(pos - mlm::vec2(x0, y0), g00);
	float		d01 = mlm::dot(pos - mlm::vec2(x0, y1), g01);
	float		d10 = mlm::dot(pos - mlm::vec2(x1, y0), g10);
	float		d11 = mlm::dot(pos - mlm::vec2(x1, y1), g11);

	float		sx = _smoothStep(x - static_cast<float>(x0));
	float		sy = _smoothStep(y - static_cast<float>(y0));

	float		value = std::lerp(std::lerp(d00, d10, sx), std::lerp(d01, d11, sx), sy);
	return (value);
}

float		Perlin::_smoothStep(float t) const
{
	return (t * t * t * (t * (t * 6 - 15) + 10));
}

mlm::vec2	Perlin::_gradient(int x, int y) const
{
	uint64_t	hash = _hash(x, y);
	float		angle = mlm::radians(static_cast<float>(hash % 360));
	return (mlm::vec2(std::cos(angle), std::sin(angle)));
}

uint64_t	Perlin::_hash(int x, int y) const
{
	uint64_t	ret = _seed;
	ret ^= static_cast<uint64_t>(x) * 0x9E3779B185EBCA87ULL;
	ret ^= static_cast<uint64_t>(y) * 0xC2B2AE3D27D4EB4FULL;
	ret ^= (ret >> 33);
	ret *= 0xFF51AFD7ED558CCDULL;
	ret ^= (ret >> 33);
	ret *= 0xC4CEB9FE1A85EC53ULL;
	ret ^= (ret >> 33);
	return (ret);
}	
