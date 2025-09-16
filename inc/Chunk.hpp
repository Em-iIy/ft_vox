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

		enum State {
			UNLOADED = 0,
			LOADED,
			DIRTY,
			GENERATED,
			MESHED,
			UPLOADED,
		};

		Chunk(ChunkManager &manager);
		Chunk(const mlm::ivec2 &chunkPos, ChunkManager &manager);
		~Chunk();

		void															generate();
		void															draw(Shader &shader);
		void															drawWater(Shader &shader);
		void															mesh();
		void															upload();

		Block															&getBlock(const mlm::ivec3 &blockChunkCoord);
		std::pair<mlm::vec3 &, mlm::vec3 &>								getMinMax();
		mlm::ivec2														getChunkPos();
		mlm::ivec3														getWorldPos();
		void															setState(const State state);
		State															getState() const;

	private:
		void															pushBackVertexWrapper(std::vector<Vertex> &vertices, const Vertex &vert);
		void															addCube(std::vector<Vertex> &vertices, const mlm::ivec3 &ipos);

		std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z>	blocks;
		ChunkMesh														_mesh;
		ChunkMesh														_waterMesh;
		mlm::ivec2														_chunkPos;
		mlm::ivec3														_worldPos;

		ChunkManager													&_manager;

		mlm::vec3														_min = INFINITY;
		mlm::vec3														_max = -INFINITY;

		State															_state = UNLOADED;
};