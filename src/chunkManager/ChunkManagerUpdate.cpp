/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "ChunkManager.hpp"
#include "VoxEngine.hpp"
#include "Coords.hpp"

void	ChunkManager::update()
{
	_updateLoadList();
	_updateGenerateList();
	_updateMeshList();
	_updateUnloadList();
	_updateUploadList();
	_updateVisibleList();
	_updateRenderList();
	_updateShadowRenderList();
	_updateCameraChunkCoord();
}

void	ChunkManager::_updateLoadList()
{
	int	loadCount = 0;
	for (const mlm::ivec2 &pos : chunkLoadList)
	{
		if (loadCount >= _maxLoad)
			break ;
		if (_loadChunk(pos) == true)
		{
			loadCount++;
			_updateVisibility = true;
		}
	}
	chunkLoadList.clear();
}

void	ChunkManager::_updateGenerateList()
{
	int	generateCount = 0;
	for (std::shared_ptr<Chunk> chunk : chunkGenerateList)
	{
		if (generateCount >= _maxGenerate)
			break ;
		if (chunk && chunk->getState() == Chunk::LOADED)
		{
			if (chunk->_busy == false)
			{
				chunk->_busy = true;
				_addToQueue(chunk, ChunkTask::Type::GENERATE);
			}
			generateCount++;
			_updateVisibility = true;
		}
	}
	chunkGenerateList.clear();
}

void	ChunkManager::_updateMeshList()
{
	const std::vector<mlm::ivec2>	neighbors = {
		mlm::ivec2(0, 1),
		mlm::ivec2(0, -1),
		mlm::ivec2(1, 0),
		mlm::ivec2(-1, 0),
	};
	int	remeshCount = 0;
	for (std::shared_ptr<Chunk> chunk : chunkMeshList)
	{
		if (remeshCount >= _maxMesh)
			break ;
		if (chunk && (chunk->getState() == Chunk::GENERATED || chunk->_dirty == true))
		{
			uint64_t	neighborSetupCount = 0;
			for (const mlm::ivec2 &neighbor : neighbors)
			{
				chunksMtx.lock();
				std::shared_ptr<Chunk>	chunkNeighbor = chunks[chunk->getChunkPos() - neighbor];
				chunksMtx.unlock();
				if (!chunkNeighbor || chunkNeighbor->getState() < Chunk::DIRTY)
					break ;
				neighborSetupCount++;
			}
			if (neighborSetupCount == neighbors.size())
			{
				if (chunk->_busy == false)
				{
					chunk->_busy = true;
					_addToQueue(chunk, ChunkTask::Type::MESH);
				}
				remeshCount++;
				_updateVisibility = true;
			}
		}
	}
	chunkMeshList.clear();
}

void	ChunkManager::_updateUnloadList()
{
	for (std::shared_ptr<Chunk> chunk : chunkUnloadList)
	{
		if (chunk && chunk->getState() != Chunk::UNLOADED)
		{
			_unloadChunk(chunk);
			_updateVisibility = true;
		}
	}
	if (chunkUnloadList.size() > 0)
	{
		chunkUnloadList.clear();
	}
}

void	ChunkManager::_updateUploadList()
{
	for (std::shared_ptr<Chunk> chunk : chunkUploadList)
	{
		if (chunk)
		{
			chunk->upload();
			_updateVisibility = true;
		}
	}
	chunkUploadList.clear();
}

void	ChunkManager::_updateVisibleList()
{
	if (!_updateVisibility)
		return ;
	chunkVisibleList.clear();
	// Loop through all chunks, and unload all outisde of render distance
	chunksMtx.lock();
	for (auto &[chunkCoord, chunk]: chunks)
	{
		if (!chunk)
			continue;
		if (
			(chunkCoord.x >= _renderMin.x && chunkCoord.y >= _renderMin.y)
			&& (chunkCoord.x <= _renderMax.x && chunkCoord.y <= _renderMax.y)
		)
			continue ;
		if (chunk->_busy == true)
			continue ;
		chunk->_busy = true;
		chunkUnloadList.push_back(chunk);
	}
	chunksMtx.unlock();
	// loop through all chunk coordinates within render distance
	for (int dist = 0; dist <= _renderDistance; ++dist)
	{
		for (int x = -dist; x <= dist; ++x)
		{
			for (int y = -dist; y <= dist; ++y)
			{
				// Check only perimeter of chunks at dist
				if ((x != -dist && x != dist) && (y != -dist && y != dist))
					y = dist;
				const mlm::ivec2		chunkCoord = _cameraChunkCoord + mlm::ivec2(x, y);
				chunksMtx.lock();
				std::shared_ptr<Chunk>	chunk = chunks[chunkCoord];
				chunksMtx.unlock();
				if (chunk == nullptr)
				{
					chunkLoadList.push_back(chunkCoord);
				}
				else
				{
					switch (chunk->getState())
					{
					case Chunk::LOADED:
						chunkGenerateList.push_back(chunk);
						break ;
					case Chunk::GENERATED:
						chunkMeshList.push_back(chunk);
						break ;
					case Chunk::MESHED:
						chunkUploadList.push_back(chunk);
						break ;
					case Chunk::UPLOADED:
						chunkVisibleList.push_back(chunk);
						break ;
					default:
						break;
					}
					if (chunk->_dirty == true)
						chunkMeshList.push_back(chunk);
					if (chunk->_readyToUpload == true)
						chunkUploadList.push_back(chunk);
				}
			}
		}
	}
	_updateVisibility = false;
}

void	ChunkManager::_updateRenderList()
{
	mlm::vec3	cameraPos = _engine.getCamera().getPos();
	chunkRenderList.clear();
	for (std::shared_ptr<Chunk> chunk : chunkVisibleList)
	{
		if (chunk->getState() == Chunk::UPLOADED)
		{
			const auto [min, max] = chunk->getMinMax();
			mlm::vec3 chunkToCamPos = static_cast<mlm::vec3>(chunk->getWorldPos()) - cameraPos;
			if (_engine.getFrustum().isBoxVisible(AABB(min + chunkToCamPos, max + chunkToCamPos)) == true)
				chunkRenderList.push_back(chunk);
		}
	}
}

void	ChunkManager::_updateShadowRenderList()
{
	mlm::vec3	cameraPos = _engine.getCamera().getPos();
	chunkShadowRenderList.clear();
	for (std::shared_ptr<Chunk> chunk : chunkVisibleList)
	{
		if (chunk->getState() == Chunk::UPLOADED)
		{
			const auto [min, max] = chunk->getMinMax();
			mlm::vec3 chunkToCamPos = static_cast<mlm::vec3>(chunk->getWorldPos()) - cameraPos;
			if (_engine.getShadowFrustum().isBoxVisible(AABB(min + chunkToCamPos, max + chunkToCamPos)) == true)
				chunkShadowRenderList.push_back(chunk);
		}
	}
}

void	ChunkManager::_updateCameraChunkCoord()
{
	const mlm::ivec2 &cameraChunkCoord = getChunkCoord(_engine.getCamera().getPos());
	// Check if the camera entered a new chunk
	if (cameraChunkCoord != _cameraChunkCoord)
	{
		_cameraChunkCoord = cameraChunkCoord;

		// Update chunk visibility lists
		_updateVisibility = true;
		// Set the new min and max rendered chunk coordinates
		_renderMin = _cameraChunkCoord - mlm::ivec2(_renderDistance);
		_renderMax = _cameraChunkCoord + mlm::ivec2(_renderDistance);
	}
}
