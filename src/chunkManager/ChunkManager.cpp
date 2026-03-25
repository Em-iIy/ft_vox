/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "Settings.hpp"
#include "ChunkManager.hpp"
#include "VoxEngine.hpp"
#include "Coords.hpp"
#include "Logger.hpp"

#include <memory>

int	getChunkCount();

bool	ChunkManager::_loadChunk(const mlm::ivec2 &chunkCoord)
{
	_chunksMtx.lock();
	if (_chunks[chunkCoord] != nullptr)
	{
		_chunksMtx.unlock();
		return (false);
	}
	_chunksMtx.unlock();

	std::shared_ptr<Chunk>	chunk = std::make_shared<Chunk>(chunkCoord, *this);
	_chunksMtx.lock();
	_chunks[chunkCoord] = std::move(chunk);
	_chunksMtx.unlock();
	return (true);
}

void	ChunkManager::_unloadChunk(std::shared_ptr<Chunk> &chunk)
{
	if (!chunk)
		return ;
	const mlm::ivec2	&chunkCoord = chunk->getChunkPos();
	_chunksMtx.lock();
	_chunks.erase(chunkCoord);
	_chunksMtx.unlock();
}

void	ChunkManager::_ThreadRoutine()
{
	while(_running)
	{
		// Periodically check for tasks in the queue
		_queueMtx.lock();
		if (_queue.empty())
		{
			_queueMtx.unlock();
			usleep(100);
			continue ;
		}

		// Access task from queue
		ChunkTask task = _popFromQueue();
		_queueMtx.unlock();

		// Attempt to convert the tasks chunk weak_ptr to shared_ptr
		std::shared_ptr<Chunk> chunk = task.ptr.lock();
		// If this returns nullptr, the chunk has since been unloaded
		if (!chunk)
		{
			Logger::log("tried to access unloaded chunk!");
			continue ;
		}
		// Run appropiate task
		switch (task.type)
		{
			case ChunkTask::Type::GENERATE:
				chunk->generate(std::atomic_load(&_generator));
				break;
			case ChunkTask::Type::MESH:
				chunk->mesh();
				break;
		}
		_updateVisibility = true;
	}
}

void	ChunkManager::_addToQueue(std::shared_ptr<Chunk> &chunk, ChunkTask::Type type)
{
	_queueMtx.lock();
	_queue.push_back({chunk, type});
	_queueMtx.unlock();
}

ChunkManager::ChunkTask	ChunkManager::_popFromQueue()
{
	ChunkManager::ChunkTask ret = _queue.front();
	_queue.pop_front();
	return (ret);
}

void	ChunkManager::renderChunks(Shader &shader)
{
	// Logger::info("t" + std::to_string(getChunkCount()) + " l" + std::to_string(_chunkLoadList.size()) + " g" + std::to_string(_chunkGenerateList.size()) + " m" + std::to_string(_chunkMeshList.size()) + " un" + std::to_string(_chunkUnloadList.size()) + " up" + std::to_string(_chunkUploadList.size()) + " v" + std::to_string(_chunkVisibleList.size()) + " r" + std::to_string(_chunkRenderList.size()));
	for (auto it = _chunkRenderList.rbegin(); it != _chunkRenderList.rend(); it++)
	{
		(*it)->draw(shader);
	}
}

void	ChunkManager::renderChunksShadows(Shader &shader)
{
	for (auto it = _chunkShadowRenderList.rbegin(); it != _chunkShadowRenderList.rend(); it++)
	{
		(*it)->draw(shader);
	}
}

void	ChunkManager::renderWater(Shader &shader)
{
	for (auto it = _chunkRenderList.rbegin(); it != _chunkRenderList.rend(); it++)
	{
		(*it)->drawWater(shader);
	}
}

void	ChunkManager::renderClear()
{
	_chunkRenderList.clear();
}
