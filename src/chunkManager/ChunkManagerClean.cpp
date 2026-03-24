/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "ChunkManager.hpp"

ChunkManager::~ChunkManager()
{}

void	ChunkManager::cleanup()
{
	// Join threads before clearing chunks to avoid heap-use-after-free on chunks
	_running = false;
	for (auto &thread : _threads)
		thread.join();
	// Clear chunk lists before chunk map
	_chunkLoadList.clear();
	_chunkGenerateList.clear();
	_chunkMeshList.clear();
	_chunkUnloadList.clear();
	_chunkVisibleList.clear();
	_chunkRenderList.clear();
	_chunkShadowRenderList.clear();

	_chunks.clear();
}
