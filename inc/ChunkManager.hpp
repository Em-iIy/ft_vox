/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include "Chunk.hpp"

#include <unordered_map>
#include <expected>

class ChunkManager {
	public:
		std::unordered_map<mlm::ivec2, Chunk, ivec2Hash>	chunks;

		void						init();
		void						render(Shader &shader);


		std::expected<Block *, int>	getBlock(const mlm::ivec3 &blockCoord);
		bool						isBlockTransparent(const mlm::ivec3 &blockCoord);
};