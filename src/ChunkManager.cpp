/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "ChunkManager.hpp"

#include <memory>

const int	MAX_LOAD_PER_FRAME = 2;
const int	MAX_SETUP_PER_FRAME = 2;
const int	MAX_BUILD_PER_FRAME = 2;
const int	MAX_UPDATE_PER_FRAME = 5;

int	getChunkCount();

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

ChunkManager::~ChunkManager()
{
}


void						ChunkManager::init()
{
	for (int x = -5; x <= 5; x++)
	{
		for (int y = -5; y <= 5; y++)
		{
			mlm::ivec2				pos(x, y);
			std::shared_ptr<Chunk>	chunk = std::make_shared<Chunk>(pos, *this);
			if (chunk)
			{
				chunk->generate();
				chunks[pos] = chunk;
			}
		}
	}
	for (auto &[_, chunk]: chunks)
	{
		if (chunk)
			chunk->update();
	}
}

void						ChunkManager::update()
{
	_updateLoadList();
	_updateSetupList();
	_updateRebuildList();
	_updateUnloadList();
	_updateFlagList();
	_updateVisibleList();
	_updateRenderList();
}

void						ChunkManager::_updateLoadList()
{
	int	loadCount = 0;
	for (const mlm::ivec2 &pos : chunkLoadList)
	{
		if (loadCount >= MAX_LOAD_PER_FRAME)
			break ;
		if (_loadChunk(pos) == true)
		{
			loadCount++;
			_updateVisibility = true;
		}
	}
	chunkLoadList.clear();
}

void						ChunkManager::_updateSetupList()
{
	int	setupCount = 0;
	for (std::shared_ptr<Chunk> chunk : chunkSetupList)
	{
		if (setupCount >= MAX_SETUP_PER_FRAME)
			break ;
		if (chunk && chunk->isLoaded() && !chunk->isSetup())
		{
			chunk->generate();
			setupCount++;
			_updateVisibility = true;
		}
	}
	chunkSetupList.clear();
}

void						ChunkManager::_updateRebuildList()
{
	const std::vector<mlm::ivec2>	neighbors = {
		mlm::ivec2(0, 1),
		mlm::ivec2(0, -1),
		mlm::ivec2(1, 0),
		mlm::ivec2(-1, 0),
	};
	int	rebuildCount = 0;
	for (std::shared_ptr<Chunk> chunk : chunkRebuildList)
	{
		if (rebuildCount >= MAX_BUILD_PER_FRAME)
			break ;
		if (chunk && chunk->isLoaded() && chunk->isSetup())
		{
			uint64_t	neighborSetupCount = 0;
			for (const mlm::ivec2 &neighbor : neighbors)
			{
				std::shared_ptr<Chunk>	chunkNeighbor = chunks[chunk->_chunkPos - neighbor];
				if (!chunkNeighbor || !chunkNeighbor->isSetup())
					break ;
				// if (chunkNeighbor && chunkNeighbor->isBuilt())
				// 	chunkUpdateFlagList.insert(chunkNeighbor);
				neighborSetupCount++;
			}
			if (neighborSetupCount == neighbors.size())
			{
				chunk->update();
				rebuildCount++;
				_updateVisibility = true;
			}
		}
	}
	chunkRebuildList.clear();
}

void						ChunkManager::_updateUnloadList()
{
	for (std::shared_ptr<Chunk> chunk : chunkUnloadList)
	{
		if (chunk && chunk->isLoaded())
		{
			_unloadChunk(chunk);
			_updateVisibility = true;
		}
	}
	chunkUnloadList.clear();
}

void						ChunkManager::_updateFlagList()
{
	int	updateCount = 0;
	std::vector<std::shared_ptr<Chunk>>	temp;
	temp.reserve(MAX_UPDATE_PER_FRAME);
	for (std::shared_ptr<Chunk> chunk : chunkUpdateFlagList)
	{
		if (updateCount >= MAX_UPDATE_PER_FRAME)
			break ;
		chunk->update();
		temp.push_back(chunk);
		updateCount++;
	}
	for (std::shared_ptr<Chunk> &chunk : temp)
		chunkUpdateFlagList.erase(chunk);
}

void						ChunkManager::_updateVisibleList()
{
	if (!_updateVisibility)
		return ;
	chunkVisibleList.clear();
	// std::cout << "Updating visibility" << std::endl;
	const mlm::ivec2	camera_pos(0);
	const int			render_distance = 10;

	const mlm::ivec2	render_min = camera_pos - mlm::ivec2(render_distance);
	const mlm::ivec2	render_max = camera_pos + mlm::ivec2(render_distance);
	for (int x = render_min.x; x <= render_max.x; ++x)
	{
		for (int y = render_min.y; y <= render_max.y; ++y)
		{
			const mlm::ivec2		chunkCoord(x, y);
			std::shared_ptr<Chunk>	chunk = chunks[chunkCoord];
			if (chunk == nullptr)
			{
				chunkLoadList.push_back(chunkCoord);
			}
			else
			{
				if (chunk->isSetup() == false)
					chunkSetupList.push_back(chunk);
				else if (chunk->isBuilt() == false)
					chunkRebuildList.push_back(chunk);
				else
					chunkVisibleList.push_back(chunk);
			}
		}
	}
	for (auto &[chunkCoord, chunk]: chunks)
	{
		if (!chunk)
			continue;
		if (
			(chunkCoord.x >= render_min.x && chunkCoord.y >= render_min.y)
			&& (chunkCoord.x <= render_max.x && chunkCoord.y <= render_max.y)
		)
			continue ;
		chunkUnloadList.push_back(chunk);
	}
	_updateVisibility = false;
}

void						ChunkManager::_updateRenderList()
{
	chunkRenderList.clear();
	for (std::shared_ptr<Chunk> chunk : chunkVisibleList)
	{
		if (chunk->isLoaded() && chunk->isBuilt() && chunk->isSetup())
		{
			// if in frustum
			chunkRenderList.push_back(chunk);
		}
	}
}

bool						ChunkManager::_loadChunk(const mlm::ivec2 &chunkCoord)
{
	// if loadable from file
	// load from file
	// else
	if (chunks[chunkCoord] != nullptr)
		return (false);
	// std::cout << "loading chunk " << chunkCoord << std::endl;
	std::shared_ptr<Chunk>	chunk = std::make_shared<Chunk>(chunkCoord, *this);
	chunks[chunkCoord] = std::move(chunk);
	return (true);
}

void						ChunkManager::_unloadChunk(std::shared_ptr<Chunk> &chunk)
{
	if (!chunk)
		return ;
	const mlm::ivec2	&chunkCoord = chunk->_chunkPos;
	std::cout << "unloading " << chunk->_chunkPos << std::endl;
	if (chunk.unique() != true)
	{
		std::cerr << "WARNING: Chunk " << chunkCoord << " set for unloading, but isn't unique! found " << chunk.use_count() << " times" << std::endl;
		std::cerr << "DEBUG: t" << getChunkCount() << " l" << chunkLoadList.size() << " s" << chunkSetupList.size() << " r" << chunkRebuildList.size() << " u" << chunkUnloadList.size() << " f" << chunkUpdateFlagList.size() << " v" << chunkVisibleList.size() << " v" << chunkRenderList.size() << std::endl;
	}
	chunks.erase(chunkCoord);
}

void						ChunkManager::render(Shader &shader)
{
	// for (auto &[_, chunk]: chunks)
	// std::cerr << "DEBUG: t" << getChunkCount() << " l" << chunkLoadList.size() << " s" << chunkSetupList.size() << " r" << chunkRebuildList.size() << " u" << chunkUnloadList.size() << " f" << chunkUpdateFlagList.size() << " v" << chunkVisibleList.size() << " r" << chunkRenderList.size() << std::endl;
	for (auto chunk : chunkRenderList)
	{
		chunk->draw(shader);
	}
	chunkRenderList.clear();
}

Expected<Block *, int>	ChunkManager::getBlock(const mlm::ivec3 &blockCoord)
{
	if (blockCoord.y < 0 || static_cast<uint64_t>(blockCoord.y) >= CHUNK_SIZE_Y)
		return (1);
	mlm::ivec2				chunkCoord = getChunkCoord(blockCoord);
	std::shared_ptr<Chunk>	chunk = chunks[chunkCoord];
	if (!chunk)
		return (0);
	mlm::ivec3				blockChunkCoord = getBlockChunkCoord(blockCoord);
	Block					&block = chunk->getBlock(blockChunkCoord);
	return (&block);
}

bool						ChunkManager::isBlockTransparent(const mlm::ivec3 &blockCoord)
{
	auto	result = getBlock(blockCoord);
	if (!result.hasValue())
	{
		if (result.error() == 1)
			return (false);
		return (true);
	}
	Block	*block = result.value();
	return (block->getEnabled());
}
