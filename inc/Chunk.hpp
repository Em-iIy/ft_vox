/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#pragma once

#include "Block.hpp"
#include <array>

const uint64_t	CHUNK_SIZE_X = 16; // MUST BE POWER OF 2
const uint64_t	CHUNK_SIZE_Y = 48;
const uint64_t	CHUNK_SIZE_Z = 16; // MUST BE POWER OF 2

class Chunk {
	public:
		std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z>	blocks;

		Mesh		_mesh;
		mlm::ivec2	_chunkPos;

		Chunk();
		Chunk(const mlm::ivec2 &chunkPos);
		~Chunk();

		void	addCube(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, const mlm::ivec3 &ipos);

		void	generate();
		void	draw(Shader &shader);
		void	update();

		Block	&getBlock(const mlm::ivec3 &blockChunkCoord);
};

struct ivec2Hash {
	size_t	operator()(const mlm::ivec2 &v) const
	{
		// convert 2 32 bit ints to 1 64 bit long
		size_t ret = v.x;
		ret += (static_cast<size_t>(v.y) << 32);
		return (ret);
	};
};