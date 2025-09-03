/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#pragma once

#include "Block.hpp"
#include "ChunkManager.hpp"
#include "ChunkMesh.hpp"
#include <array>
#include <math.h>

constexpr uint64_t	CHUNK_SIZE_X = 16; // MUST BE POWER OF 2
constexpr uint64_t	CHUNK_SIZE_Y = 256;
constexpr uint64_t	CHUNK_SIZE_Z = 16; // MUST BE POWER OF 2

class ChunkManager;

class Chunk {
	public:
		std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z>	blocks;

		ChunkMesh			_mesh;
		mlm::ivec2		_chunkPos;
		mlm::ivec3		_worldPos;

		mlm::vec3		_min = INFINITY;
		mlm::vec3		_max = -INFINITY;

		ChunkManager	&_manager;

		Chunk(ChunkManager &manager);
		Chunk(const mlm::ivec2 &chunkPos, ChunkManager &manager);
		~Chunk();

		void	addCube(std::vector<Vertex> &vertices, const mlm::ivec3 &ipos);

		void	generate();
		void	draw(Shader &shader);
		void	update();

		Block	&getBlock(const mlm::ivec3 &blockChunkCoord);
	
		bool	isLoaded() const;
		bool	isSetup() const;
		bool	isBuilt() const;
		
		void	requireRebuild();

	private:
		bool	_loaded = true;
		bool	_setup = false;
		bool	_built = false;
};