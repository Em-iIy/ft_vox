/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include "Chunk.hpp"
#include "Expected.hpp"

#include <unordered_map>
#include <set>
#include <memory>

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
class VoxEngine;

class ChunkManager {
	public:
		std::unordered_map<mlm::ivec2, std::shared_ptr<Chunk>, ivec2Hash>	chunks;
		std::vector<mlm::ivec2>					chunkLoadList;
		std::vector<std::shared_ptr<Chunk>>		chunkSetupList;
		std::vector<std::shared_ptr<Chunk>>		chunkRebuildList;
		std::set<std::shared_ptr<Chunk>>		chunkUpdateFlagList;
		std::vector<std::shared_ptr<Chunk>>		chunkUnloadList;
		std::vector<std::shared_ptr<Chunk>>		chunkVisibleList;
		std::vector<std::shared_ptr<Chunk>>		chunkRenderList;

		ChunkManager(VoxEngine &engine);
		~ChunkManager();

		void						cleanup();
		void						init();

		void						update();

		void						render(Shader &shader);


		Expected<Block *, int>		getBlock(const mlm::ivec3 &blockCoord);
		bool						isBlockTransparent(const mlm::ivec3 &blockCoord);
	
	private:
		VoxEngine					&_engine;
		bool						_updateVisibility = true;
		mlm::ivec2					_cameraChunkCoord;

		void						_updateLoadList();
		void						_updateSetupList();
		void						_updateRebuildList();
		void						_updateUnloadList();
		void						_updateFlagList();
		void						_updateVisibleList();
		void						_updateRenderList();

		void						_updateCameraChunkCoord();

		bool						_loadChunk(const mlm::ivec2 &chunkCoord);
		void						_unloadChunk(std::shared_ptr<Chunk> &chunk);

};