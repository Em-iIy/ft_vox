/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "ChunkManager.hpp"
#include "VoxEngine.hpp"
#include "Coords.hpp"

#include <memory>

const int	MAX_LOAD_PER_FRAME = 8;
const int	MAX_GENERATE_PER_FRAME = 8;
const int	MAX_MESH_PER_FRAME = 8;
const int	MAX_UPDATE_PER_FRAME = 8;

const int	THREAD_COUNT = 8;

const int	RENDER_DISTANCE = 10;

int	getChunkCount();

ChunkManager::ChunkManager(VoxEngine &engine): _engine(engine)
{}

ChunkManager::~ChunkManager()
{}

void						ChunkManager::cleanup()
{
	// Join threads before clearing chunks to avoid heap-use-after-free on chunks
	_running = false;
	for (auto &thread : _threads)
		thread.join();
	// Clear chunk lists before chunk map
	chunkLoadList.clear();
	chunkGenerateList.clear();
	chunkMeshList.clear();
	chunkUpdateFlagList.clear();
	chunkUnloadList.clear();
	chunkVisibleList.clear();
	chunkRenderList.clear();

	chunks.clear();
}

void						ChunkManager::init()
{
	_renderDistance = RENDER_DISTANCE + 1;
	_updateCameraChunkCoord();
	_threads.reserve(THREAD_COUNT);
	for (int i = 0; i < THREAD_COUNT; ++i)
		_threads.emplace_back(&ChunkManager::_ThreadRoutine, this);
	
	_generator.store(std::make_shared<TerrainGenerator>(Settings::loadTerrainGenerator()));
}

void						ChunkManager::update()
{
	_updateLoadList();
	_updateGenerateList();
	_updateMeshList();
	_updateUnloadList();
	_updateUploadList();
	_updateFlagList();
	_updateVisibleList();
	_updateRenderList();
	_updateCameraChunkCoord();
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

void						ChunkManager::_updateGenerateList()
{
	int	generateCount = 0;
	for (std::shared_ptr<Chunk> chunk : chunkGenerateList)
	{
		if (generateCount >= MAX_GENERATE_PER_FRAME)
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

void						ChunkManager::_updateMeshList()
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
		if (remeshCount >= MAX_MESH_PER_FRAME)
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
				if (chunk->getState() != Chunk::DIRTY && false)
				{
					if (chunkNeighbor && chunkNeighbor->getState() >= Chunk::MESHED)
						chunkUpdateFlagList.insert(chunkNeighbor);
				}
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

void						ChunkManager::_updateUnloadList()
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

void						ChunkManager::_updateUploadList()
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

void						ChunkManager::_updateFlagList()
{
	int	updateCount = 0;
	std::vector<std::shared_ptr<Chunk>>	temp;
	temp.reserve(MAX_UPDATE_PER_FRAME);
	for (std::shared_ptr<Chunk> chunk : chunkUpdateFlagList)
	{
		if (updateCount >= MAX_UPDATE_PER_FRAME)
			break ;
		chunk->setState(Chunk::DIRTY);
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

void						ChunkManager::_updateRenderList()
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

void						ChunkManager::_updateCameraChunkCoord()
{
	const mlm::ivec2 &cameraChunkCoord = getChunkCoord(_engine.getCamera().getPos());
	if (cameraChunkCoord != _cameraChunkCoord)
	{
		_cameraChunkCoord = cameraChunkCoord;
		_updateVisibility = true;

		_renderMin = _cameraChunkCoord - mlm::ivec2(_renderDistance);
		_renderMax = _cameraChunkCoord + mlm::ivec2(_renderDistance);
	}
}

bool						ChunkManager::_loadChunk(const mlm::ivec2 &chunkCoord)
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

void						ChunkManager::_unloadChunk(std::shared_ptr<Chunk> &chunk)
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

void						ChunkManager::renderChunks(Shader &shader)
{
	// std::cerr << "DEBUG: t" << getChunkCount() << " l" << chunkLoadList.size() << " g" << chunkGenerateList.size() << " m" << chunkMeshList.size() << " un" << chunkUnloadList.size() << " up" << chunkUploadList.size() << " f" << chunkUpdateFlagList.size() << " v" << chunkVisibleList.size() << " r" << chunkRenderList.size() << std::endl;
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

void						ChunkManager::renderWater(Shader &shader)
{
	for (auto it = chunkRenderList.rbegin(); it != chunkRenderList.rend(); it++)
	{
		(*it)->drawWater(shader);
	}
}

void						ChunkManager::renderClear()
{
	chunkRenderList.clear();
}

void						ChunkManager::unloadAll()
{
	chunksMtx.lock();
	chunks.clear();
	_generator.store(std::make_shared<TerrainGenerator>(Settings::loadTerrainGenerator()));
	chunksMtx.unlock();
	_updateVisibility = true;
}

Expected<Block, int>	ChunkManager::getBlock(const mlm::vec3 &blockCoord)
{
	return (getBlock(getWorldCoord(blockCoord)));
}

Expected<Block, int>	ChunkManager::getBlock(const mlm::ivec3 &blockCoord)
{
	if (blockCoord.y < 0 || static_cast<uint64_t>(blockCoord.y) >= CHUNK_SIZE_Y)
		return (1);
	mlm::ivec2				chunkCoord = getChunkCoord(blockCoord);
	chunksMtx.lock();
	std::shared_ptr<Chunk>	chunk = chunks[chunkCoord];
	chunksMtx.unlock();
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
	if (blockCoord.y < 0 || static_cast<uint64_t>(blockCoord.y) >= CHUNK_SIZE_Y)
		return ;
	mlm::ivec2				chunkCoord = getChunkCoord(blockCoord);
	chunksMtx.lock();
	std::shared_ptr<Chunk>	chunk = chunks[chunkCoord];
	chunksMtx.unlock();
	if (!chunk)
		return ;
	mlm::ivec3				blockChunkCoord = getBlockChunkCoord(blockCoord);

	bool updated = chunk->setBlock(blockChunkCoord, block);
	if (updated == false)
		return ;
	// std::cout << "set block: " << blockCoord << " " << blockChunkCoord << std::endl;
	if (blockChunkCoord.x == 0)
	{
		chunksMtx.lock();
		chunks[chunkCoord + mlm::ivec2(-1, 0)]->_dirty = true;
		chunksMtx.unlock();
	}
	if (blockChunkCoord.z == 0)
	{
		chunksMtx.lock();
		chunks[chunkCoord + mlm::ivec2(0, -1)]->_dirty = true;
		chunksMtx.unlock();
	}
	if (blockChunkCoord.x == CHUNK_SIZE_X - 1)
	{
		chunksMtx.lock();
		chunks[chunkCoord + mlm::ivec2(1, 0)]->_dirty = true;
		chunksMtx.unlock();
	}
	if (blockChunkCoord.z == CHUNK_SIZE_Z - 1)
	{
		chunksMtx.lock();
		chunks[chunkCoord + mlm::ivec2(0, 1)]->_dirty = true;
		chunksMtx.unlock();
	}
	chunk->_dirty = true;
	_updateVisibility = true;
}

Expected<Block::Type, int>	ChunkManager::getBlockType(const mlm::vec3 &blockCoord)
{
	return (getBlockType(getWorldCoord(blockCoord)));
}

Expected<Block::Type, int>	ChunkManager::getBlockType(const mlm::ivec3 &blockCoord)
{
	if (blockCoord.y < 0 || static_cast<uint64_t>(blockCoord.y) >= CHUNK_SIZE_Y)
		return (1);
	mlm::ivec2				chunkCoord = getChunkCoord(blockCoord);
	chunksMtx.lock();
	std::shared_ptr<Chunk>	chunk = chunks[chunkCoord];
	chunksMtx.unlock();
	if (!chunk)
		return (0);
	mlm::ivec3				blockChunkCoord = getBlockChunkCoord(blockCoord);
	Block::Type				blockType = chunk->getBlockType(blockChunkCoord);
	return (blockType);
}

bool						ChunkManager::isBlockTransparent(const mlm::vec3 &blockCoord)
{
	return (isBlockTransparent(getWorldCoord(blockCoord)));
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
		mlm::vec3	rayWorldPos = cameraPos + cameraViewDir * (step * stepSize);
		mlm::ivec3	rayWorldCoord = getWorldCoord(rayWorldPos);
		if (!isBlockTransparent(rayWorldCoord))
			return (rayWorldCoord);
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
		mlm::vec3	rayWorldPos = cameraPos + cameraViewDir * (step * stepSize);
		mlm::ivec3	rayWorldCoord = getWorldCoord(rayWorldPos);
		if (!isBlockTransparent(rayWorldCoord))
			return (ret);
		ret = rayWorldCoord;
	}
	return (false);
}

void	ChunkManager::placeBlock(Block block)
{
	Expected<mlm::ivec3, bool>	rayWorldCoord = castRayExcluding();
	if (rayWorldCoord.hasValue() && rayWorldCoord.value() != getWorldCoord(_engine.getCamera().getPos()))
	{
		setBlock(rayWorldCoord.value(), block);
	}
}

void	ChunkManager::deleteBlock()
{
	Expected<mlm::ivec3, bool>	rayWorldCoord = castRayIncluding();
	if (rayWorldCoord.hasValue())
	{
		setBlock(rayWorldCoord.value(), Block::AIR);
	}
}

void						ChunkManager::setUpdateVisibility()
{
	_updateVisibility = true;
}

VoxEngine					&ChunkManager::getEngine()
{
	return (_engine);
}
