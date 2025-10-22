/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include "Chunk.hpp"
#include "Expected.hpp"
#include "TerrainGenerator.hpp"
#include "Settings.hpp"

#include <unordered_map>
#include <set>
#include <memory>
#include <thread>
#include <atomic>
#include <queue>
#include <functional>

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
		using ChunkCallback = std::function<void()>;
		struct ChunkTask
		{
			std::weak_ptr<Chunk>				ptr;
			enum class Type {GENERATE, MESH}	type;
		};

		ChunkManager(VoxEngine &engine);
		~ChunkManager();

		void																cleanup();
		void																init();

		void																update();
		void																renderChunks(Shader &shader);
		void																renderWater(Shader &shader);
		void																renderClear();

		void																unloadAll();

		Expected<Block, int>												getBlock(const mlm::vec3 &blockCoord);
		Expected<Block, int>												getBlock(const mlm::ivec3 &blockCoord);

		void																setBlock(const mlm::vec3 &blockCoord, Block block);
		void																setBlock(const mlm::ivec3 &blockCoord, Block block);

		Expected<Block::Type, int>											getBlockType(const mlm::vec3 &blockCoord);
		Expected<Block::Type, int>											getBlockType(const mlm::ivec3 &blockCoord);

		bool																isBlockTransparent(const mlm::vec3 &blockCoord);
		bool																isBlockTransparent(const mlm::ivec3 &blockCoord);
	
		Expected<mlm::ivec3, bool>											castRayIncluding();
		Expected<mlm::ivec3, bool>											castRayExcluding();
		void																placeBlock(Block block);
		void																deleteBlock();

		void																setUpdateVisibility();

		VoxEngine															&getEngine();

	private:
		std::unordered_map<mlm::ivec2, std::shared_ptr<Chunk>, ivec2Hash>	chunks;
		std::mutex															chunksMtx;
		std::vector<mlm::ivec2>												chunkLoadList = {};
		std::vector<std::shared_ptr<Chunk>>									chunkGenerateList = {};
		std::vector<std::shared_ptr<Chunk>>									chunkMeshList = {};
		std::vector<std::shared_ptr<Chunk>>									chunkUnloadList = {};
		std::vector<std::shared_ptr<Chunk>>									chunkUploadList = {};
		std::vector<std::shared_ptr<Chunk>>									chunkVisibleList = {};
		std::vector<std::shared_ptr<Chunk>>									chunkRenderList = {};

		// Multithreading stuff
		std::deque<ChunkTask>												_queue;
		std::mutex															_queueMtx;
		std::vector<std::thread>											_threads;
		std::atomic<bool>													_running = true;

		VoxEngine															&_engine;

		std::atomic<TerrainGeneratorPtr>									_generator;

		std::atomic<bool>													_updateVisibility = true;
		mlm::ivec2															_cameraChunkCoord = {2147483647};
		int																	_renderDistance = {};
		mlm::ivec2															_renderMin = {0};
		mlm::ivec2															_renderMax = {0};

		void																_updateLoadList();
		void																_updateGenerateList();
		void																_updateMeshList();
		void																_updateUnloadList();
		void																_updateUploadList();
		void																_updateVisibleList();
		void																_updateRenderList();

		void																_updateCameraChunkCoord();
		
		bool																_loadChunk(const mlm::ivec2 &chunkCoord);
		void																_unloadChunk(std::shared_ptr<Chunk> &chunk);

		void																_ThreadRoutine();
		void																_addToQueue(std::shared_ptr<Chunk> &chunk, ChunkTask::Type type);
		ChunkTask															_popFromQueue();

};