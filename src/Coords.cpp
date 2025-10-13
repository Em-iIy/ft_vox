/*
Created by: Emily (Em_iIy) Winnink
Created on: 13/10/2025
*/

#include "Coords.hpp"

constexpr int LOG2_CHUNK_SIZE_X = std::bit_width(CHUNK_SIZE_X) - 1;
constexpr int LOG2_CHUNK_SIZE_Z = std::bit_width(CHUNK_SIZE_Z) - 1;

mlm::ivec2	getChunkCoord(const mlm::ivec3 &worldCoord)
{
	return mlm::ivec2(
		worldCoord.x >> LOG2_CHUNK_SIZE_X,
		worldCoord.z >> LOG2_CHUNK_SIZE_Z
	);
}

mlm::ivec3	getBlockChunkCoord(const mlm::ivec3 &worldCoord)
{
	return mlm::ivec3(
		worldCoord.x & (CHUNK_SIZE_X - 1),
		worldCoord.y,
		worldCoord.z & (CHUNK_SIZE_Z - 1)
	);
}

mlm::ivec3	getWorldCoord(const mlm::vec3 &coord)
{
	return mlm::ivec3(
		static_cast<int>(std::floor(coord.x)),
		static_cast<int>(std::floor(coord.y)),
		static_cast<int>(std::floor(coord.z))
	);
}

uint64_t	index3D(uint64_t x, uint64_t y, uint64_t z)
{
	return (z * (CHUNK_SIZE_Y * CHUNK_SIZE_X) + y * CHUNK_SIZE_X + x);
}

uint64_t	index3D(const mlm::ivec3 &coord)
{
	return (coord.z * (CHUNK_SIZE_Y * CHUNK_SIZE_X) + coord.y * CHUNK_SIZE_X + coord.x);
}