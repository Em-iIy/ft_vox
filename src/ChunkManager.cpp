/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "ChunkManager.hpp"

int	roundUp(int num, int mult)
{
	return ((num + mult - 1) & -mult);
}

constexpr int LOG2_CHUNK_SIZE_X = std::bit_width(CHUNK_SIZE_X) - 1;
constexpr int LOG2_CHUNK_SIZE_Z = std::bit_width(CHUNK_SIZE_Z) - 1;

mlm::ivec2 getChunkCoord(const mlm::ivec3 &worldCoord)
{
	return mlm::ivec2(
		worldCoord.x >> LOG2_CHUNK_SIZE_X,
		worldCoord.z >> LOG2_CHUNK_SIZE_Z
	);
}

mlm::ivec3 getBlockChunkCoord(const mlm::ivec3 &worldCoord)
{
	return mlm::ivec3(
		worldCoord.x & (CHUNK_SIZE_X - 1),
		worldCoord.y,
		worldCoord.z & (CHUNK_SIZE_Z - 1)
	);
}

void						ChunkManager::init()
{
	for (int x = -10; x <= 10; x++)
	{
		for (int y = -10; y <= 10; y++)
		{
			mlm::ivec2	pos(x, y);
			chunks.try_emplace(pos, pos, *this);
			try
			{
				Chunk	&chunk = chunks.at(pos);
				chunk.generate();
			}
			catch(const std::exception& e)
			{
			}	
		}
	}
	// std::cout << "size: " << chunks.size() << std::endl;
	for (auto &[_, chunk]: chunks)
	{
		chunk.update();
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
	if (blockCoord.y < 0 || static_cast<uint64_t>(blockCoord.y) >= CHUNK_SIZE_Y)
		return (std::unexpected(0));
	try
	{
		mlm::ivec2 chunkCoord = getChunkCoord(blockCoord);
		Chunk &chunk = chunks.at(chunkCoord);
		mlm::ivec3 blockChunkCoord = getBlockChunkCoord(blockCoord);
		// std::cout << "chunkCoord: " << chunkCoord << " " << "blockChunkCoord: " << blockChunkCoord << " ";
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
	// std::cout << "coord: " << blockCoord << " ";
	auto result = getBlock(blockCoord);
	if (!result.has_value())
	{
		// std::cout << std::endl;
		return (true);
	}
	Block	*block = result.value();
	// std::cout << block << std::endl;
	return (block->getEnabled());
}
