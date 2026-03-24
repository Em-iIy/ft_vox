/*
Created by: Emily (Em_iIy) Winnink
Created on: 23/03/2026
*/

#include "Chunk.hpp"
#include "Coords.hpp"

void	Chunk::generate(TerrainGeneratorPtr generator)
{
	_busyMtx.lock();

	perlinSamplers samplers = generator->getSamplers();

	for (uint64_t x = 0; x < CHUNK_SIZE_X; ++x)
	{
		for (uint64_t z = 0; z < CHUNK_SIZE_Z; ++z)
		{
			int	terrainHeight = generator->getTerrainHeight(samplers, mlm::ivec2(x + _worldPos.x, z + _worldPos.z));
			for (uint64_t y = 0; y < CHUNK_SIZE_Y; ++y)
			{
				mlm::ivec3	pos = _worldPos + mlm::ivec3(x, y, z);
				uint64_t	index = index3D(x, y, z);
				Block		block = generator->getBlock(samplers, pos, terrainHeight);
				_blockMtx.lock();
				_blocks[index] = block;
				_blockMtx.unlock();
			}
		}
	}
	setState(GENERATED);
	_busyMtx.unlock();
	_busy = false;
}
