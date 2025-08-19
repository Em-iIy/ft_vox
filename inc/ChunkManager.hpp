/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include "Chunk.hpp"

#include <unordered_map>
#include <expected>

struct ivec2Hash {
	size_t	operator()(const mlm::ivec2 &v) const
	{
		// convert 2 32 bit ints to 1 64 bit long
		size_t ret = v.x;
		ret += (static_cast<size_t>(v.y) << 32);
		return (ret);
	};
};

class Chunk;

class ChunkManager {
	public:
		std::unordered_map<mlm::ivec2, Chunk, ivec2Hash>	chunks;

		void						init();
		void						render(Shader &shader);


		std::expected<Block *, int>	getBlock(const mlm::ivec3 &blockCoord);
		bool						isBlockTransparent(const mlm::ivec3 &blockCoord);
};