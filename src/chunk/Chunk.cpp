/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#include "Chunk.hpp"
#include "Perlin.hpp"
#include "Spline.hpp"
#include "VoxEngine.hpp"
#include "Coords.hpp"

std::atomic<int> chunk_count = 0;

int	getChunkCount()
{
	return (chunk_count);
}

Chunk::Chunk(ChunkManager &manager): _manager(manager)
{
	std::cout << "default constructor" << std::endl;
}

Chunk::Chunk(const mlm::ivec2 &chunkPos, ChunkManager &manager): _chunkPos(chunkPos), _manager(manager)
{
	_worldPos = mlm::ivec3(CHUNK_SIZE_X * _chunkPos.x, 0, CHUNK_SIZE_Z * _chunkPos.y);
	setState(LOADED);
	chunk_count++;
}

Chunk::~Chunk()
{
	_busyMtx.lock();
	if (getState() == UPLOADED || getState() == DIRTY)
	{
		_mesh.del();
		_waterMesh.del();
	}
	_busyMtx.unlock();
	chunk_count--;
}


void	Chunk::draw(Shader &shader)
{
	mlm::mat4 model(1.0f);
	model = mlm::translate(model, static_cast<mlm::vec3>(_worldPos) - _manager.getEngine().getCamera().getPos());
	shader.set_mat4("uModel", model);
	_busyMtx.lock();
	_mesh.draw(shader);
	_busyMtx.unlock();
}

void	Chunk::drawWater(Shader &shader)
{
	mlm::mat4 model(1.0f);
	model = mlm::translate(model, static_cast<mlm::vec3>(_worldPos) - _manager.getEngine().getCamera().getPos());
	shader.set_mat4("uModel", model);
	_busyMtx.lock();
	_waterMesh.draw(shader);
	_busyMtx.unlock();
}

void	Chunk::upload()
{
	if (_readyToUpload == false)
		return ;
	_busyMtx.lock();
	_mesh.setup_mesh();
	_waterMesh.setup_mesh();
	_busyMtx.unlock();
	_readyToUpload = false;
	setState(UPLOADED);
}
