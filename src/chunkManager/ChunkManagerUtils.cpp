/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "ChunkManager.hpp"
#include "Coords.hpp"
#include "Settings.hpp"

// Check weather the y coordinate is in valid range
static bool	checkValidYCoordinate(const float y)
{
	return (y < 0 || static_cast<uint64_t>(y) >= CHUNK_SIZE_Y);
}

void	ChunkManager::unloadAll()
{
	_chunksMtx.lock();
	_chunks.clear();
	try
	{
		// Reload the terrain generator from settings
		TerrainGeneratorPtr newGenerator = std::make_shared<TerrainGenerator>(Settings::loadTerrainGenerator());
		_generator.store(newGenerator);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	_chunksMtx.unlock();
	_updateVisibility = true;
}

Expected<Block, int>	ChunkManager::getBlock(const mlm::vec3 &blockCoord)
{
	return (getBlock(getWorldCoord(blockCoord)));
}

Expected<Block, int>	ChunkManager::getBlock(const mlm::ivec3 &blockCoord)
{
	if (checkValidYCoordinate(blockCoord.y))
		return (1);
	mlm::ivec2				chunkCoord = getChunkCoord(blockCoord);
	_chunksMtx.lock();
	std::shared_ptr<Chunk>	chunk = _chunks[chunkCoord];
	_chunksMtx.unlock();
	if (!chunk)
		return (0);
	mlm::ivec3				blockChunkCoord = getBlockChunkCoord(blockCoord);
	Block					block = chunk->getBlock(blockChunkCoord);
	return (block);
}

void	ChunkManager::setBlock(const mlm::vec3 &blockCoord, Block block)
{
	setBlock(getWorldCoord(blockCoord), block);
}

void	ChunkManager::setBlock(const mlm::ivec3 &blockCoord, Block block)
{
	if (checkValidYCoordinate(blockCoord.y))
		return ;
	mlm::ivec2				chunkCoord = getChunkCoord(blockCoord);
	_chunksMtx.lock();
	std::shared_ptr<Chunk>	chunk = _chunks[chunkCoord];
	_chunksMtx.unlock();
	if (!chunk)
		return ;
	mlm::ivec3				blockChunkCoord = getBlockChunkCoord(blockCoord);

	bool updated = chunk->setBlock(blockChunkCoord, block);
	if (updated == false)
		return ;

	// If on chunk boundary -> set neighbor dirty flag for remeshing
	if (blockChunkCoord.x == 0)
	{
		_chunksMtx.lock();
		_chunks[chunkCoord + mlm::ivec2(-1, 0)]->_dirty = true;
		_chunksMtx.unlock();
	}
	if (blockChunkCoord.z == 0)
	{
		_chunksMtx.lock();
		_chunks[chunkCoord + mlm::ivec2(0, -1)]->_dirty = true;
		_chunksMtx.unlock();
	}
	if (blockChunkCoord.x == CHUNK_SIZE_X - 1)
	{
		_chunksMtx.lock();
		_chunks[chunkCoord + mlm::ivec2(1, 0)]->_dirty = true;
		_chunksMtx.unlock();
	}
	if (blockChunkCoord.z == CHUNK_SIZE_Z - 1)
	{
		_chunksMtx.lock();
		_chunks[chunkCoord + mlm::ivec2(0, 1)]->_dirty = true;
		_chunksMtx.unlock();
	}
	// Set chunk dirty flag for remeshing
	chunk->_dirty = true;
	_updateVisibility = true;
}

Expected<Block::Type, int>	ChunkManager::getBlockType(const mlm::vec3 &blockCoord)
{
	return (getBlockType(getWorldCoord(blockCoord)));
}

Expected<Block::Type, int>	ChunkManager::getBlockType(const mlm::ivec3 &blockCoord)
{
	if (checkValidYCoordinate(blockCoord.y))
		return (1);

	// Fetch chunk
	mlm::ivec2				chunkCoord = getChunkCoord(blockCoord);
	_chunksMtx.lock();
	std::shared_ptr<Chunk>	chunk = _chunks[chunkCoord];
	_chunksMtx.unlock();
	if (!chunk)
		return (0);

	mlm::ivec3				blockChunkCoord = getBlockChunkCoord(blockCoord);
	Block::Type				blockType = chunk->getBlockType(blockChunkCoord);
	return (blockType);
}

bool	ChunkManager::isBlockTransparent(const mlm::vec3 &blockCoord)
{
	return (isBlockTransparent(getWorldCoord(blockCoord)));
}

bool	ChunkManager::isBlockTransparent(const mlm::ivec3 &blockCoord)
{
	auto	result = getBlock(blockCoord);
	if (!result.hasValue())
	{
		if (result.error() == 1)
			return (false);
		return (true);
	}
	Block	block = result.value();
	return (block.getTransparent());
}

Expected<mlm::ivec3, bool>	ChunkManager::castRayIncluding()
{
	const float	stepSize = 0.25f;
	const int	stepDepth = 50;

	mlm::vec3	cameraPos = _engine.getCamera().getPos();
	mlm::vec3	cameraViewDir = _engine.getCamera().getViewDir();

	for (int step = 0; step < stepDepth; ++step)
	{
		// Get position of the ray at the new depth
		mlm::vec3	rayWorldPos = cameraPos + cameraViewDir * (step * stepSize);
		// Check if the block at this position is transparent
		mlm::ivec3	rayWorldCoord = getWorldCoord(rayWorldPos);
		if (!isBlockTransparent(rayWorldCoord))
			return (rayWorldCoord); // return the block coordinate
	}
	return (false);
}

Expected<mlm::ivec3, bool>	ChunkManager::castRayExcluding()
{
	const float	stepSize = 0.25f;
	const int	stepDepth = 50;

	mlm::vec3	cameraPos = _engine.getCamera().getPos();
	mlm::vec3	cameraViewDir = _engine.getCamera().getViewDir();
	mlm::ivec3	ret = getWorldCoord(cameraPos);
	for (int step = 0; step < stepDepth; ++step)
	{
		// Get position of the ray at the new depth
		mlm::vec3	rayWorldPos = cameraPos + cameraViewDir * (step * stepSize);
		// Check if the block at this position is transparent
		mlm::ivec3	rayWorldCoord = getWorldCoord(rayWorldPos);
		if (!isBlockTransparent(rayWorldCoord))
			return (ret); // return previous block coordinate
		ret = rayWorldCoord;
	}
	return (false);
}

void	ChunkManager::placeBlock(Block block)
{
	// Get the block coordinate before the looked at block
	Expected<mlm::ivec3, bool>	rayWorldCoord = castRayExcluding();
	if (rayWorldCoord.hasValue() && rayWorldCoord.value() != getWorldCoord(_engine.getCamera().getPos()))
	{
		setBlock(rayWorldCoord.value(), block);
	}
}

void	ChunkManager::deleteBlock()
{
	// Get the block coordinate of the looked at block
	Expected<mlm::ivec3, bool>	rayWorldCoord = castRayIncluding();
	if (rayWorldCoord.hasValue())
	{
		setBlock(rayWorldCoord.value(), Block::AIR);
	}
}

void	ChunkManager::setUpdateVisibility()
{
	_updateVisibility = true;
}

VoxEngine	&ChunkManager::getEngine()
{
	return (_engine);
}
