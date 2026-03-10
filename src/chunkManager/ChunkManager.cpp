/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "Settings.hpp"
#include "ChunkManager.hpp"
#include "VoxEngine.hpp"
#include "Coords.hpp"

#include <memory>

int	getChunkCount();

bool	ChunkManager::_loadChunk(const mlm::ivec2 &chunkCoord)
{
	// if loadable from file
	// load from file
	// else
	chunksMtx.lock();
	if (chunks[chunkCoord] != nullptr)
		return (false);
	chunksMtx.unlock();
	std::shared_ptr<Chunk>	chunk = std::make_shared<Chunk>(chunkCoord, *this);
	chunksMtx.lock();
	chunks[chunkCoord] = std::move(chunk);
	chunksMtx.unlock();
	return (true);
}

void	ChunkManager::_unloadChunk(std::shared_ptr<Chunk> &chunk)
{
	if (!chunk)
		return ;
	const mlm::ivec2	&chunkCoord = chunk->getChunkPos();
	chunksMtx.lock();
	chunks.erase(chunkCoord);
	chunksMtx.unlock();
}

void	ChunkManager::_ThreadRoutine()
{
	// std::cout << "starting thread" << std::endl;
	while(_running)
	{
		_queueMtx.lock();
		if (_queue.empty())
		{
			_queueMtx.unlock();
			usleep(100);
			continue ;
		}
		ChunkTask task = _popFromQueue();
		_queueMtx.unlock();
		std::shared_ptr<Chunk> chunk = task.ptr.lock();
		if (!chunk)
		{
			std::cout << "tried to access unloaded chunk!" << std::endl;
			continue ;
		}
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
	// std::cerr << "DEBUG: t" << getChunkCount() << " l" << chunkLoadList.size() << " g" << chunkGenerateList.size() << " m" << chunkMeshList.size() << " un" << chunkUnloadList.size() << " up" << chunkUploadList.size() << " v" << chunkVisibleList.size() << " r" << chunkRenderList.size() << std::endl;
	// _queueMtx.lock();
	// std::cerr << "queue size: " << _queue.size() << std::endl;
	// _queueMtx.unlock();
	// glEnable(GL_CULL_FACE);
	for (auto it = chunkRenderList.rbegin(); it != chunkRenderList.rend(); it++)
	{
		(*it)->draw(shader);
	}
	// glDisable(GL_CULL_FACE);
}

void	ChunkManager::renderChunksShadows(Shader &shader)
{
	for (auto it = chunkShadowRenderList.rbegin(); it != chunkShadowRenderList.rend(); it++)
	{
		(*it)->draw(shader);
	}
}

void	ChunkManager::renderWater(Shader &shader)
{
	for (auto it = chunkRenderList.rbegin(); it != chunkRenderList.rend(); it++)
	{
		(*it)->drawWater(shader);
	}
}

void	ChunkManager::renderClear()
{
	chunkRenderList.clear();
}
