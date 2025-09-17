/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "ChunkManager.hpp"
#include "VoxEngine.hpp"

#include <memory>

const int	MAX_LOAD_PER_FRAME = 10;
const int	MAX_GENERATE_PER_FRAME = 10;
const int	MAX_MESH_PER_FRAME = 10;
const int	MAX_UPDATE_PER_FRAME = 8;

const int	THREAD_COUNT = 8;

const int	RENDER_DISTANCE = 10;

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
				_addToQueue(std::bind(&Chunk::generate, chunk));
			}
			// float start = glfwGetTime();
			// chunk->generate();
			// std::cout << glfwGetTime() - start << std::endl;
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
	int	rebuildCount = 0;
	for (std::shared_ptr<Chunk> chunk : chunkMeshList)
	{
		if (rebuildCount >= MAX_MESH_PER_FRAME)
			break ;
		if (chunk && (chunk->getState() == Chunk::GENERATED || chunk->getState() == Chunk::DIRTY))
		{
			uint64_t	neighborSetupCount = 0;
			for (const mlm::ivec2 &neighbor : neighbors)
			{
				std::shared_ptr<Chunk>	chunkNeighbor = chunks[chunk->getChunkPos() - neighbor];
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
					_addToQueue(std::bind(&Chunk::mesh, chunk));
				}
				// chunk->mesh();
				rebuildCount++;
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
		if (chunk && chunk->getState() != Chunk::UNLOADED && chunk->_busy == false)
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
				std::shared_ptr<Chunk>	chunk = chunks[chunkCoord];
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
					case Chunk::DIRTY:
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
					// if (chunk->isSetup() == false)
					// 	chunkGenerateList.push_back(chunk);
					// else if (chunk->isBuilt() == false)
					// 	chunkMeshList.push_back(chunk);
					// else
					// 	chunkVisibleList.push_back(chunk);
				}
			}
		}
	}
	// Loop through all chunks, and unload all outisde of render distance
	for (auto &[chunkCoord, chunk]: chunks)
	{
		if (!chunk)
			continue;
		if (
			(chunkCoord.x >= _renderMin.x && chunkCoord.y >= _renderMin.y)
			&& (chunkCoord.x <= _renderMax.x && chunkCoord.y <= _renderMax.y)
		)
			continue ;
		chunkUnloadList.push_back(chunk);
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
	if (chunks[chunkCoord] != nullptr)
		return (false);
	std::shared_ptr<Chunk>	chunk = std::make_shared<Chunk>(chunkCoord, *this);
	chunks[chunkCoord] = std::move(chunk);
	return (true);
}

void						ChunkManager::_unloadChunk(std::shared_ptr<Chunk> &chunk)
{
	if (!chunk)
		return ;
	const mlm::ivec2	&chunkCoord = chunk->getChunkPos();
	chunks.erase(chunkCoord);
}


bool	checkEmpty(std::deque<ChunkManager::ChunkCallback> &queue, std::mutex &mtx)
{
	bool ret;
	mtx.lock();
	ret = queue.empty();
	mtx.unlock();
	return (ret);
}

void	ChunkManager::_ThreadRoutine()
{
	std::cout << "starting thread" << std::endl;
	while(_running)
	{
		_queueMtx.lock();
		if (_queue.empty() == false)
		{
			auto func = _popFromQueue();
			_queueMtx.unlock();
			func();
		}
		else
		{
			_queueMtx.unlock();
			usleep(100);
		}
	}
}

void	ChunkManager::_addToQueue(ChunkManager::ChunkCallback callback)
{
	_queueMtx.lock();
	_queue.push_back(callback);
	_queueMtx.unlock();
}

ChunkManager::ChunkCallback	ChunkManager::_popFromQueue()
{
	ChunkManager::ChunkCallback ret = _queue.front();
	_queue.pop_front();
	return (ret);
}

void						ChunkManager::render(Shader &shader)
{
	std::cerr << "DEBUG: t" << getChunkCount() << " l" << chunkLoadList.size() << " g" << chunkGenerateList.size() << " m" << chunkMeshList.size() << " un" << chunkUnloadList.size() << " up" << chunkUploadList.size() << " f" << chunkUpdateFlagList.size() << " v" << chunkVisibleList.size() << " r" << chunkRenderList.size() << std::endl;
	_queueMtx.lock();
	std::cerr << "queue size: " << _queue.size() << std::endl;
	_queueMtx.unlock();
	// glEnable(GL_CULL_FACE);
	for (auto it = chunkRenderList.rbegin(); it != chunkRenderList.rend(); it++)
	{
		(*it)->draw(shader);
	}
	// glDisable(GL_CULL_FACE);
	for (auto it = chunkRenderList.rbegin(); it != chunkRenderList.rend(); it++)
	{
		(*it)->drawWater(shader);
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
	return (block->getTransparent());
}

void						ChunkManager::setUpdateVisibility()
{
	_updateVisibility = true;
}

VoxEngine					&ChunkManager::getEngine()
{
	return (_engine);
}