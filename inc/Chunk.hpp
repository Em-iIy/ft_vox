/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#pragma once

#include "Block.hpp"
#include "ChunkManager.hpp"
#include "ChunkMesh.hpp"
#include <array>

constexpr uint64_t	CHUNK_SIZE_X = 16; // MUST BE POWER OF 2
constexpr uint64_t	CHUNK_SIZE_Y = 48;
constexpr uint64_t	CHUNK_SIZE_Z = 16; // MUST BE POWER OF 2

class ChunkManager;

class Chunk {
	public:
		std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z>	blocks;

		ChunkMesh			_mesh;
		mlm::ivec2		_chunkPos;

		ChunkManager	&_manager;

		Chunk(ChunkManager &manager);
		Chunk(const mlm::ivec2 &chunkPos, ChunkManager &manager);
		~Chunk();

		void	addCube(std::vector<Vertex> &vertices, const mlm::ivec3 &ipos);

		void	generate();
		void	draw(Shader &shader);
		void	update();

		Block	&getBlock(const mlm::ivec3 &blockChunkCoord);
};