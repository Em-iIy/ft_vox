/*
Created by: Emily (Em_iIy) Winnink
Created on: 13/10/2025
*/

#include "Chunk.hpp"

/*
// Get coordinates of chunk containing requested block
*/
mlm::ivec2	getChunkCoord(const mlm::ivec3 &worldCoord);

/*
// Get coordinates local to the chunk containing requested block
*/
mlm::ivec3	getBlockChunkCoord(const mlm::ivec3 &worldCoord);

/*
// Get coordinates of the block containing these floating point coordinates
*/
mlm::ivec3	getWorldCoord(const mlm::vec3 &coord);

/*
// Get 1 dimensional index of 3 dimensional coordinates local to chunk
*/
uint64_t	index3D(uint64_t x, uint64_t y, uint64_t z);

/*
// Get 1 dimensional index of 3 dimensional coordinates local to chunk
*/
uint64_t	index3D(const mlm::ivec3 &coord);
