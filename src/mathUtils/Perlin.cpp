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

float	Perlin::getValue(float x, float y, float z) const
{
	mlm::vec3	pos(x, y, z);
	// std::cout << _hash(x, y, z) << std::endl;
	// std::cout << _hash(x, y) << std::endl;
	// std::cout << _hash(y, z) << std::endl;
	// return (_hash(x, y, z));
	int			x0 = static_cast<int>(std::floor(x));
	int			x1 = x0 + 1;
	int			y0 = static_cast<int>(std::floor(y));
	int			y1 = y0 + 1;
	int			z0 = static_cast<int>(std::floor(z));
	int			z1 = z0 + 1;

	mlm::vec3	g000 = _gradient(x0, y0, z0);
	mlm::vec3	g001 = _gradient(x0, y0, z1);
	mlm::vec3	g010 = _gradient(x0, y1, z0);
	mlm::vec3	g011 = _gradient(x0, y1, z1);
	mlm::vec3	g100 = _gradient(x1, y0, z0);
	mlm::vec3	g101 = _gradient(x1, y0, z1);
	mlm::vec3	g110 = _gradient(x1, y1, z0);
	mlm::vec3	g111 = _gradient(x1, y1, z1);

	float		d000 = mlm::dot(pos - mlm::vec3(x0, y0, z0), g000);
	float		d001 = mlm::dot(pos - mlm::vec3(x0, y0, z1), g001);
	float		d010 = mlm::dot(pos - mlm::vec3(x0, y1, z0), g010);
	float		d011 = mlm::dot(pos - mlm::vec3(x0, y1, z1), g011);
	float		d100 = mlm::dot(pos - mlm::vec3(x1, y0, z0), g100);
	float		d101 = mlm::dot(pos - mlm::vec3(x1, y0, z1), g101);
	float		d110 = mlm::dot(pos - mlm::vec3(x1, y1, z0), g110);
	float		d111 = mlm::dot(pos - mlm::vec3(x1, y1, z1), g111);
	
	float		sx = _smoothStep(x - static_cast<float>(x0));
	float		sy = _smoothStep(y - static_cast<float>(y0));
	float		sz = _smoothStep(z - static_cast<float>(z0));

	float		xLerp00 = std::lerp(d000, d100, sx);
	float		xLerp01 = std::lerp(d001, d101, sx);
	float		xLerp10 = std::lerp(d010, d110, sx);
	float		xLerp11 = std::lerp(d011, d111, sx);

	float		yLerp0 = std::lerp(xLerp00, xLerp10, sy);
	float		yLerp1 = std::lerp(xLerp01, xLerp11, sy);

	float		value = std::lerp(yLerp0, yLerp1, sz);
	return (value);
}

float	Perlin::_smoothStep(float t) const
{
	return (t * t * t * (t * (t * 6 - 15) + 10));
}

mlm::vec2	Perlin::_gradient(int x, int y) const
{
	uint64_t	hash = _hash(x, y);
	float		angle = mlm::radians(static_cast<float>(hash % 360));
	return (mlm::vec2(std::cos(angle), std::sin(angle)));
}

mlm::vec3	Perlin::_gradient(int x, int y, int z) const
{
	uint64_t	hash = _hash(x, y, z);
	float theta = static_cast<float>(hash & 0xFFFFFFFFULL) / static_cast<float>(0xFFFFFFFFULL) * 2.0f * M_PI;
	float phi   = static_cast<float>(hash >> 32) / static_cast<float>(0xFFFFFFFFULL) * M_PI;
	return (mlm::vec3(std::cos(theta) * std::sin(phi), std::sin(theta) * std::sin(phi), std::cos(phi)));
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

uint64_t	Perlin::_hash(int x, int y, int z) const
{
	uint64_t	ret = _seed;
	ret ^= static_cast<uint64_t>(x) * 0x9E3779B185EBCA87ULL;
	ret ^= static_cast<uint64_t>(y) * 0xC2B2AE3D27D4EB4FULL;
	ret ^= static_cast<uint64_t>(z) * 0x165667B19E3779F9ULL;
	ret ^= (ret >> 33);
	ret *= 0xFF51AFD7ED558CCDULL;
	ret ^= (ret >> 33);
	ret *= 0xC4CEB9FE1A85EC53ULL;
	ret ^= (ret >> 33);
	return (ret);
}