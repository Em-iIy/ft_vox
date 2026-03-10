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
	chunkLoadList.clear();
	chunkGenerateList.clear();
	chunkMeshList.clear();
	chunkUnloadList.clear();
	chunkVisibleList.clear();
	chunkRenderList.clear();
	chunkShadowRenderList.clear();

	chunks.clear();
}
