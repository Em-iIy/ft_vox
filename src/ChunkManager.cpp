/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "ChunkManager.hpp"

int	roundUp(int num, int mult)
{
	return ((num + mult - 1) & -mult);
}

mlm::ivec2	getChunkCoord(const mlm::ivec3 &worldCoord)
{
	return (mlm::ivec2(roundUp(worldCoord.x, CHUNK_SIZE_X) / CHUNK_SIZE_X - 1, roundUp(worldCoord.z, CHUNK_SIZE_Z) / CHUNK_SIZE_Z) - 1);
}

mlm::ivec3	getBlockChunkCoord(const mlm::ivec3 &worldCoord)
{
	return (worldCoord - mlm::ivec3(roundUp(worldCoord.x, CHUNK_SIZE_X), 0, roundUp(worldCoord.z, CHUNK_SIZE_Z)));
}

void						ChunkManager::init()
{
	for (int x = -5; x <= 5; x++)
	{
		for (int y = -5; y <= 5; y++)
		{
			mlm::ivec2	pos(x, y);
			chunks[pos] = Chunk(pos);
			chunks[pos].generate();
		}
	}
}

void						ChunkManager::render(Shader &shader)
{
	for (auto &[_, chunk]: chunks)
	{
		chunk.draw(shader);
	}
}

std::expected<Block *, int>	ChunkManager::getBlock(const mlm::ivec3 &blockCoord)
{
	try
	{
		mlm::ivec2 chunkCoord = getChunkCoord(blockCoord);
		Chunk &chunk = chunks.at(chunkCoord);
		mlm::ivec3 blockChunkCoord = getBlockChunkCoord(blockCoord);
		Block	&block = chunk.getBlock(blockChunkCoord);
		return (&block);
	}
	catch(const std::exception& e)
	{
	}
	return (std::unexpected(0));
}

bool						ChunkManager::isBlockTransparent(const mlm::ivec3 &blockCoord)
{
	auto result = getBlock(blockCoord);
	if (!result.has_value())
		return (false);
	return (result.value()->getEnabled());
}
