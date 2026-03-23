/*
Created by: Emily (Em_iIy) Winnink
Created on: 23/03/2026
*/

#include "Chunk.hpp"
#include "Coords.hpp"

Block	Chunk::getBlock(const mlm::ivec3 &blockChunkCoord)
{
	_blockMtx.lock();
	Block ret = blocks[index3D(blockChunkCoord)];
	_blockMtx.unlock();
	return (ret);
}

bool	Chunk::setBlock(const mlm::ivec3 &blockChunkCoord, Block block)
{
	bool ret = true;
	_blockMtx.lock();
	Block &target = blocks[index3D(blockChunkCoord)];
	if (target.getType() == block.getType())
		ret = false;
	target = block;
	_blockMtx.unlock();
	return (ret);
}

Block::Type	Chunk::getBlockType(const mlm::ivec3 &blockChunkCoord)
{
	_blockMtx.lock();
	Block::Type ret = blocks[index3D(blockChunkCoord)].getType();
	_blockMtx.unlock();
	return (ret);
}

std::pair<mlm::vec3 &, mlm::vec3 &>	Chunk::getMinMax()
{
	return (std::make_pair(std::reference_wrapper(_min), std::reference_wrapper(_max)));
}

mlm::ivec2	Chunk::getChunkPos()
{
	return (_chunkPos);
}

mlm::ivec3	Chunk::getWorldPos()
{
	return (_worldPos);
}

void	Chunk::setState(const Chunk::State state)
{
	_stateMtx.lock();
	_state = state;
	_stateMtx.unlock();
}

Chunk::State	Chunk::getState()
{
	_stateMtx.lock();
	State ret = _state;
	_stateMtx.unlock();
	return (ret);
}
