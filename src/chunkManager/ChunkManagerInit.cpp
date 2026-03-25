/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "ChunkManager.hpp"
#include "VoxEngine.hpp"
#include "Settings.hpp"
#include "Logger.hpp"

ChunkManager::ChunkManager(VoxEngine &engine): _engine(engine)
{}

void	ChunkManager::init(const ChunkManagerDTO &dto)
{
	_renderDistance = static_cast<int>(dto.renderDistance + 1);
	_threadCount = static_cast<int>(dto.threadCount);
	_maxLoad = static_cast<int>(dto.maxLoad);
	_maxGenerate = static_cast<int>(dto.maxGenerate);
	_maxMesh = static_cast<int>(dto.maxMesh);

	_updateCameraChunkCoord();
	_threads.reserve(_threadCount);
	// Create shared pointer for the terrain generator used by all the chunks
	Logger::info("Loading terrain generator");
	_generator.store(std::make_shared<TerrainGenerator>(Settings::loadTerrainGenerator()));
	Logger::info("Creating threads");
	for (int i = 0; i < _threadCount; ++i)
		_threads.emplace_back(&ChunkManager::_ThreadRoutine, this);
}
