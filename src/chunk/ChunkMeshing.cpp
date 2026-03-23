/*
Created by: Emily (Em_iIy) Winnink
Created on: 23/03/2026
*/

#include "Chunk.hpp"
#include "VoxEngine.hpp"
#include "Coords.hpp"

enum Faces {
	TOP,
	BACK,
	FRONT,
	LEFT,
	RIGHT,
	BOTTOM
};

enum Corners {
	BACK_BOTTOM_LEFT,
	BACK_BOTTOM_RIGHT,
	BACK_TOP_LEFT,
	BACK_TOP_RIGHT,
	FRONT_BOTTOM_LEFT,
	FRONT_BOTTOM_RIGHT,
	FRONT_TOP_LEFT,
	FRONT_TOP_RIGHT,
};

enum UvCorners {
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
};

static bool		shouldDrawFace(Expected<Block, int> &neighborResult, Block &block)
{
	if (!neighborResult.hasValue())
	{
		if (neighborResult.error() == 1)
			return (true);
		return (false);
	}
	Block neighbor = neighborResult.value();
	if (block.getType() == Block::WATER && neighbor.getType() == Block::AIR)
		return (true);
	if (block.getTransparent() == false && neighbor.getTransparent() == true)
		return (true);
	return (false);
}

// Added vertex to vertices while keeping track of the min and max values found so far
void	Chunk::pushBackVertexWrapper(std::vector<Vertex> &vertices, const Vertex &vert)
{
	const mlm::vec3 &vec = vert.pos;
	_min.x = std::min(_min.x, vec.x);
	_min.y = std::min(_min.y, vec.y);
	_min.z = std::min(_min.z, vec.z);

	_max.x = std::max(_max.x, vec.x);
	_max.y = std::max(_max.y, vec.y);
	_max.z = std::max(_max.z, vec.z);
	vertices.push_back(vert);
}

void	Chunk::addCube(std::vector<Vertex> &vertices, const mlm::ivec3 &ipos)
{
	static const mlm::ivec3	neighbors[] = {
		mlm::ivec3(0, 1, 0),
		mlm::ivec3(0, 0, -1),
		mlm::ivec3(0, 0, 1),
		mlm::ivec3(-1, 0, 0),
		mlm::ivec3(1, 0, 0),
		mlm::ivec3(0, -1, 0),
	};

	static const mlm::vec3	normals[] = {
		mlm::vec3(0.0f, 1.0f, 0.0f),
		mlm::vec3(0.0f, 0.0f, -1.0f),
		mlm::vec3(0.0f, 0.0f, 1.0f),
		mlm::vec3(-1.0f, 0.0f, 0.0f),
		mlm::vec3(1.0f, 0.0f, 0.0f),
		mlm::vec3(0.0f, -1.0f, 0.0f),
	};

	mlm::ivec3	worldPos = _worldPos + ipos;
	mlm::vec3	pos(ipos);
	const mlm::vec3	positions[] = {
		mlm::vec3(0.0f, 0.0f, 0.0f) + pos, // 0 back bottom left
		mlm::vec3(1.0f, 0.0f, 0.0f) + pos, //  1 back bottom right
		mlm::vec3(0.0f, 1.0f, 0.0f) + pos, //  2 back top left
		mlm::vec3(1.0f, 1.0f, 0.0f) + pos, //   3 back top right
		mlm::vec3(0.0f, 0.0f, 1.0f) + pos, //  4 front bottom left
		mlm::vec3(1.0f, 0.0f, 1.0f) + pos, //   5 front bottom right
		mlm::vec3(0.0f, 1.0f, 1.0f) + pos, //   6 front top left
		mlm::vec3(1.0f, 1.0f, 1.0f) + pos, //    7 front top right
	};
	Block		block = getBlock(ipos);
	Atlas		&atlas = _manager.getEngine().getAtlas();
	const std::vector<mlm::vec2>	&offsets = atlas.getOffset(block.getType());
	const std::vector<mlm::vec2>	&uvCorners = atlas.getCorners();

	std::vector<Expected<Block, int>> blockNeighbors;
	for (const mlm::ivec3 &neighbor : neighbors)
	{
		mlm::ivec3	neighborIpos = ipos + neighbor;
		// Check wether neighbor is within chunk or not
		if (
			neighborIpos.x < 0 || neighborIpos.x >= static_cast<int>(CHUNK_SIZE_X) ||
			neighborIpos.y < 0 || neighborIpos.y >= static_cast<int>(CHUNK_SIZE_Y) ||
			neighborIpos.z < 0 || neighborIpos.z >= static_cast<int>(CHUNK_SIZE_Z)
		)
			blockNeighbors.push_back(_manager.getBlock(worldPos + neighbor));
		else
			blockNeighbors.push_back(getBlock(neighborIpos));
	}

	// top face
	if (shouldDrawFace(blockNeighbors[TOP], block) == true)
	{
		mlm::vec3	normal = normals[TOP] * 0.9f;
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_LEFT],		normal, offsets[TOP] + uvCorners[TOP_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_LEFT],		normal, offsets[TOP] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_RIGHT],	normal, offsets[TOP] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_LEFT],		normal, offsets[TOP] + uvCorners[TOP_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_RIGHT],	normal, offsets[TOP] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_RIGHT],		normal, offsets[TOP] + uvCorners[TOP_RIGHT]});
	}
	// back face
	if (shouldDrawFace(blockNeighbors[BACK], block) == true)
	{
		mlm::vec3	normal = normals[BACK] * 0.7f;
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_LEFT],	normal, offsets[BACK] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_LEFT],		normal, offsets[BACK] + uvCorners[TOP_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_RIGHT],	normal, offsets[BACK] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_RIGHT],	normal, offsets[BACK] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_LEFT],		normal, offsets[BACK] + uvCorners[TOP_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_RIGHT],		normal, offsets[BACK] + uvCorners[TOP_LEFT]});
	}
	// front face
	if (shouldDrawFace(blockNeighbors[FRONT], block) == true)
	{
		mlm::vec3	normal = normals[FRONT] * 0.7f;
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_LEFT],	normal, offsets[FRONT] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_RIGHT],	normal, offsets[FRONT] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_LEFT],		normal, offsets[FRONT] + uvCorners[TOP_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_RIGHT],	normal, offsets[FRONT] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_RIGHT],	normal, offsets[FRONT] + uvCorners[TOP_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_LEFT],		normal, offsets[FRONT] + uvCorners[TOP_LEFT]});
	}
	// left face
	if (shouldDrawFace(blockNeighbors[LEFT], block) == true)
	{
		mlm::vec3	normal = normals[LEFT] * 0.8f;
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_LEFT],	normal, offsets[LEFT] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_LEFT],	normal, offsets[LEFT] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_LEFT],		normal, offsets[LEFT] + uvCorners[TOP_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_LEFT],	normal, offsets[LEFT] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_LEFT],		normal, offsets[LEFT] + uvCorners[TOP_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_LEFT],		normal, offsets[LEFT] + uvCorners[TOP_LEFT]});
	}
	// right face
	if (shouldDrawFace(blockNeighbors[RIGHT], block) == true)
	{
		mlm::vec3	normal = normals[RIGHT] * 0.8f;
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_RIGHT],	normal, offsets[RIGHT] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_RIGHT],	normal, offsets[RIGHT] + uvCorners[TOP_LEFT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_RIGHT],	normal, offsets[RIGHT] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_RIGHT],	normal, offsets[RIGHT] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[BACK_TOP_RIGHT],		normal, offsets[RIGHT] + uvCorners[TOP_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_TOP_RIGHT],	normal, offsets[RIGHT] + uvCorners[TOP_LEFT]});
	}
	// bottom face
	if (shouldDrawFace(blockNeighbors[BOTTOM], block) == true)
	{
		mlm::vec3	normal = normals[BOTTOM] * 0.9f;
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_RIGHT],	normal, offsets[BOTTOM] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_LEFT],	normal, offsets[BOTTOM] + uvCorners[TOP_LEFT]});
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_LEFT],	normal, offsets[BOTTOM] + uvCorners[BOTTOM_LEFT]});
		pushBackVertexWrapper(vertices, {positions[BACK_BOTTOM_RIGHT],	normal, offsets[BOTTOM] + uvCorners[BOTTOM_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_RIGHT],	normal, offsets[BOTTOM] + uvCorners[TOP_RIGHT]});
		pushBackVertexWrapper(vertices, {positions[FRONT_BOTTOM_LEFT],	normal, offsets[BOTTOM] + uvCorners[TOP_LEFT]});
	}
}

void	Chunk::mesh()
{
	// float start = glfwGetTime();
	_busyMtx.lock();
	std::vector<Vertex> vertices;
	std::vector<Vertex> waterVertices;
	for (uint64_t x = 0; x < CHUNK_SIZE_X; ++x)
	{
		for (uint64_t y = 0; y < CHUNK_SIZE_Y; ++y)
		{
			for (uint64_t z = 0; z < CHUNK_SIZE_Z; ++z)
			{
				uint64_t	index = index3D(x, y, z);
				mlm::ivec3	pos(x, y, z);
				_blockMtx.lock();
				Block		&block = blocks[index];
				_blockMtx.unlock();
				if (block.getEnabled())
				{
					if (block.getType() == Block::WATER)
						addCube(waterVertices, pos);
					else
						addCube(vertices, pos);
				}
			}
		}
	}
	_mesh.get_vertices() = vertices;
	_waterMesh.get_vertices() = waterVertices;
	if (getState() < MESHED)
		setState(MESHED);
	// if (_dirty == true)
	// 	std::cout << "remeshed: " << _chunkPos << std::endl;
	_dirty = false;
	_readyToUpload = true;
	_busyMtx.unlock();
	_busy = false;
	// std::cout << glfwGetTime() - start << std::endl;
}
