/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "ChunkManager.hpp"
#include "VoxEngine.hpp"
#include "Coords.hpp"

void	ChunkManager::update()
{
	// Update chunk states
	_updateLoadList();
	_updateGenerateList();
	_updateMeshList();
	_updateUnloadList();
	_updateUploadList();

	// Determine what chunks go where
	_updateVisibleList();

	// Update rendering lists
	_updateRenderList();
	_updateShadowRenderList();
	_updateCameraChunkCoord();
}

void	ChunkManager::_updateLoadList()
{
	int	loadCount = 0;
	for (const mlm::ivec2 &pos : _chunkLoadList)
	{
		if (loadCount >= _maxLoad)
			break ;
		if (_loadChunk(pos) == true)
		{
			loadCount++;
			_updateVisibility = true;
		}
	}
	_chunkLoadList.clear();
}

void	ChunkManager::_updateGenerateList()
{
	int	generateCount = 0;
	for (std::shared_ptr<Chunk> chunk : _chunkGenerateList)
	{
		if (generateCount >= _maxGenerate)
			break ;
		if (chunk && chunk->getState() == Chunk::LOADED)
		{
			// Only generate chunk if it isn't in the queue
			if (chunk->_busy == false)
			{
				chunk->_busy = true;
				_addToQueue(chunk, ChunkTask::Type::GENERATE);
			}
			generateCount++;
			_updateVisibility = true;
		}
	}
	_chunkGenerateList.clear();
}

void	ChunkManager::_updateMeshList()
{
	const std::vector<mlm::ivec2>	neighbors = {
		mlm::ivec2(0, 1),
		mlm::ivec2(0, -1),
		mlm::ivec2(1, 0),
		mlm::ivec2(-1, 0),
	};
	int	meshCount = 0;
	for (std::shared_ptr<Chunk> chunk : _chunkMeshList)
	{
		if (meshCount >= _maxMesh)
			break ;
		// Is chunk valid target for meshing
		if (!chunk || (chunk->getState() != Chunk::GENERATED && chunk->_dirty == false))
			continue ;

		// Check whether all neighboring chunks are generated before meshing
		uint64_t	neighborSetupCount = 0;
		for (const mlm::ivec2 &neighbor : neighbors)
		{
			_chunksMtx.lock();
			std::shared_ptr<Chunk>	chunkNeighbor = _chunks[chunk->getChunkPos() - neighbor];
			_chunksMtx.unlock();
			// If neighbor is only loaded at best don't count it
			if (!chunkNeighbor || chunkNeighbor->getState() < Chunk::DIRTY)
				break ;
			neighborSetupCount++;
		}
		if (neighborSetupCount < neighbors.size())
			continue;
		// Only mesh chunk if it isn't in the queue
		if (chunk->_busy == false)
		{
			chunk->_busy = true;
			_addToQueue(chunk, ChunkTask::Type::MESH);
		}
		meshCount++;
		_updateVisibility = true;
	}
	_chunkMeshList.clear();
}

void	ChunkManager::_updateUnloadList()
{
	// Unload all chunks set for unloading
	for (std::shared_ptr<Chunk> chunk : _chunkUnloadList)
	{
		if (chunk && chunk->getState() != Chunk::UNLOADED)
		{
			_unloadChunk(chunk);
			_updateVisibility = true;
		}
	}
	if (_chunkUnloadList.size() > 0)
	{
		_chunkUnloadList.clear();
	}
}

void	ChunkManager::_updateUploadList()
{
	// Upload all chunks with meshes ready to be sent to GPU
	for (std::shared_ptr<Chunk> chunk : _chunkUploadList)
	{
		if (chunk)
		{
			chunk->upload();
			_updateVisibility = true;
		}
	}
	_chunkUploadList.clear();
}

void	ChunkManager::_updateVisibleList()
{
	if (!_updateVisibility)
		return ;
	_chunkVisibleList.clear();
	// Loop through all chunks, and unload all outisde of render distance
	_chunksMtx.lock();
	for (auto &[chunkCoord, chunk]: _chunks)
	{
		if (!chunk)
			continue;
		if (
			(chunkCoord.x >= _renderMin.x && chunkCoord.y >= _renderMin.y)
			&& (chunkCoord.x <= _renderMax.x && chunkCoord.y <= _renderMax.y)
		)
			continue ;
		// Don't unload yet if chunk is in the task queue
		if (chunk->_busy == true)
			continue ;
		chunk->_busy = true;
		_chunkUnloadList.push_back(chunk);
	}
	_chunksMtx.unlock();
	// Loop through all chunk coordinates within render distance
	// Starting at the camera chunk and expanding out from there
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
				_chunksMtx.lock();
				std::shared_ptr<Chunk>	chunk = _chunks[chunkCoord];
				_chunksMtx.unlock();
				// Load chunk if it isn't contained in the chunk map
				if (chunk == nullptr)
				{
					_chunkLoadList.push_back(chunkCoord);
					continue ;
				}
				// Place chunk in the correct list based on the current state
				switch (chunk->getState())
				{
				case Chunk::LOADED:
					_chunkGenerateList.push_back(chunk);
					break ;
				case Chunk::GENERATED:
					_chunkMeshList.push_back(chunk);
					break ;
				case Chunk::MESHED:
					_chunkUploadList.push_back(chunk);
					break ;
				case Chunk::UPLOADED:
					_chunkVisibleList.push_back(chunk);
					break ;
				default:
					break;
				}
				if (chunk->_dirty == true)
					_chunkMeshList.push_back(chunk);
				if (chunk->_readyToUpload == true)
					_chunkUploadList.push_back(chunk);
			}
		}
	}
	_updateVisibility = false;
}

void	ChunkManager::_updateRenderList()
{
	mlm::vec3	cameraPos = _engine.getCamera().getPos();
	_chunkRenderList.clear();
	for (std::shared_ptr<Chunk> chunk : _chunkVisibleList)
	{
		if (chunk->getState() == Chunk::UPLOADED)
		{
			// Create AABB around the chunk mesh
			const auto [min, max] = chunk->getMinMax();
			mlm::vec3 chunkToCamPos = static_cast<mlm::vec3>(chunk->getWorldPos()) - cameraPos;
			// Check if any of the edges of the AABB falls inside the frustum
			if (_engine.getFrustum().isBoxVisible(AABB(min + chunkToCamPos, max + chunkToCamPos)) == true)
				_chunkRenderList.push_back(chunk);
		}
	}
}

void	ChunkManager::_updateShadowRenderList()
{
	mlm::vec3	cameraPos = _engine.getCamera().getPos();
	_chunkShadowRenderList.clear();
	for (std::shared_ptr<Chunk> chunk : _chunkVisibleList)
	{
		if (chunk->getState() == Chunk::UPLOADED)
		{
			// Create AABB around the chunk mesh
			const auto [min, max] = chunk->getMinMax();
			mlm::vec3 chunkToCamPos = static_cast<mlm::vec3>(chunk->getWorldPos()) - cameraPos;
			// Check if any of the edges of the AABB falls inside the frustum
			if (_engine.getShadowFrustum().isBoxVisible(AABB(min + chunkToCamPos, max + chunkToCamPos)) == true)
				_chunkShadowRenderList.push_back(chunk);
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
