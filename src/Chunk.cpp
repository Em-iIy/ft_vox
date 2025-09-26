/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#include "Chunk.hpp"

#include "Perlin.hpp"

#include "Spline.hpp"

#include "VoxEngine.hpp"

int chunk_count = 0;
const int g_seed = 4242;

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

static uint64_t	index3D(uint64_t x, uint64_t y, uint64_t z)
{
	return (z * (CHUNK_SIZE_Y * CHUNK_SIZE_X) + y * CHUNK_SIZE_X + x);
}

static uint64_t	index3D(const mlm::ivec3 &coord)
{
	return (coord.z * (CHUNK_SIZE_Y * CHUNK_SIZE_X) + coord.y * CHUNK_SIZE_X + coord.x);
}

static bool		shouldDrawFace(Expected<Block *, int> &neighborResult, Block &block)
{
	if (!neighborResult.hasValue())
	{
		if (neighborResult.error() == 1)
			return (true);
		return (false);
	}
	Block &neighbor = *neighborResult.value();
	if (block.getType() == Block::WATER && neighbor.getType() == Block::AIR)
		return (true);
	if (block.getTransparent() == false && neighbor.getTransparent() == true)
		return (true);
	return (false);
}

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
	Block		&block = getBlock(ipos);
	Atlas		&atlas = _manager.getEngine().getAtlas();
	const std::vector<mlm::vec2>	&offsets = atlas.getOffset(block.getType());
	const std::vector<mlm::vec2>	&uvCorners = atlas.getCorners();

	std::vector<Expected<Block *, int>> blockNeighbors;
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
			blockNeighbors.push_back(&getBlock(neighborIpos));
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

float octaves(uint64_t seed, mlm::vec3 pos, uint64_t depth, float in_step)
{
	Perlin perlin;
	perlin.setSeed(seed);
	float ret = 0.0f;
	float step = 1.0f;
	for (; depth > 0; --depth)
	{
		float temp = perlin.getValue(pos.x * step, pos.z * step) / step;
		if (std::abs(temp) > std::numeric_limits<float>::epsilon())
			ret += temp;
		else
			break ;
		step *= in_step;
	}
	return (ret);
}

float octaves3(uint64_t seed, mlm::vec3 pos, uint64_t depth, float in_step)
{
	Perlin perlin;
	perlin.setSeed(seed);
	float ret = 0.0f;
	float step = 1.0f;
	for (; depth > 0; --depth)
	{
		float temp = perlin.getValue(pos.x, pos.y * step, pos.z) / step;
		if (std::abs(temp) > std::numeric_limits<float>::epsilon())
			ret += temp;
		else
			break ;
		step *= in_step;
	}
	return (ret);
}

float	continentalnessSpline(const float value)
{
	static Spline spline({
		mlm::vec2(-1.f, 250.0f),
		mlm::vec2(-0.8f, 95.0f),
		mlm::vec2(-0.2f, 95.0f),
		mlm::vec2(0.0f, 108.0f),
		mlm::vec2(0.25f, 120.0f),
		mlm::vec2(0.45f, 180.0f),
		mlm::vec2(1.00f, 200.0f),
	});
	float ret = spline.evaluate(value);
	return (ret);
}

float temp(uint64_t seed, const mlm::ivec3 &pos)
{
	return ((octaves(seed, static_cast<mlm::vec3>(pos) / 400.0f, 5, 2.0f)));
}

float temp3(uint64_t seed, const mlm::ivec3 &pos)
{
	return ((octaves3(seed, static_cast<mlm::vec3>(pos) / 160.0f, 1, 1.0f)));
	// return ((octaves3(static_cast<mlm::vec3>(pos) / 80.0f, 3, 1.5f)));
}

float temp4(uint64_t seed, const mlm::ivec3 &pos)
{
	return ((octaves3(seed, static_cast<mlm::vec3>(pos) / 160.0f, 1, 1.0f)));
	// return ((octaves3(static_cast<mlm::vec3>(pos) / 80.0f, 3, 1.5f)));
}

int	heightRand(uint64_t seed, const mlm::ivec3 &pos)
{
	int	ret = 0;
	ret += static_cast<int>(continentalnessSpline(temp(seed, pos)));
	return (ret);
}

void	Chunk::generate()
{
	_busyMtx.lock();
	const int	seaLevel = 110;
	for (uint64_t x = 0; x < CHUNK_SIZE_X; ++x)
	{
		for (uint64_t z = 0; z < CHUNK_SIZE_Z; ++z)
		{
			mlm::ivec3	iPos(x, 0, z);
			int			tempYMax = heightRand(g_seed, iPos + _worldPos);
			for (uint64_t y = 0; y < CHUNK_SIZE_Y; ++y)
			{
				iPos.y = y;
				mlm::ivec3	pos = iPos + _worldPos;
				uint64_t	index = index3D(x, y, z);
				Block::Type type = Block::STONE;
				if (iPos.y > tempYMax)
				{
					type = iPos.y <= seaLevel ? Block::WATER : Block::AIR;
					_blockMtx.lock();
					blocks[index].setType(type);
					blocks[index].setEnabled(type == Block::AIR ? false : true);
					_blockMtx.unlock();
				}
				else
				{
					if (iPos.y == tempYMax)
					{
						if (iPos.y < seaLevel)
							type = Block::DIRT;
						else
							type = Block::GRASS;
					}
					else if (iPos.y < tempYMax && iPos.y > tempYMax - 4)
						type = Block::DIRT;
					_blockMtx.lock();
					blocks[index].setType(type);
					blocks[index].setEnabled(true);
					_blockMtx.unlock();
					float value = temp3(g_seed, pos);
					float value2 = temp3(g_seed + 1, pos);
					if ((iPos.y != 0 && (std::abs(value) < 0.02f && std::abs(value2) < 0.02f)))
					{
						type = tempYMax <= seaLevel ? Block::WATER : Block::AIR;
						_blockMtx.lock();
						blocks[index].setType(type);
						blocks[index].setEnabled(type == Block::AIR ? false : true);
						_blockMtx.unlock();
					}
				}
			}
		}
	}
	setState(GENERATED);
	_busyMtx.unlock();
	_busy = false;
}

void	Chunk::draw(Shader &shader)
{
	mlm::mat4 model(1.0f);
	model = mlm::translate(model, static_cast<mlm::vec3>(_worldPos) - _manager.getEngine().getCamera().getPos());
	shader.set_mat4("model", model);
	_busyMtx.lock();
	_mesh.draw(shader);
	_busyMtx.unlock();
}

void	Chunk::drawWater(Shader &shader)
{
	mlm::mat4 model(1.0f);
	model = mlm::translate(model, static_cast<mlm::vec3>(_worldPos) - _manager.getEngine().getCamera().getPos());
	shader.set_mat4("model", model);
	_busyMtx.lock();
	_waterMesh.draw(shader);
	_busyMtx.unlock();
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
	_dirty = false;
	_readyToUpload = true;
	_busyMtx.unlock();
	_busy = false;
	// std::cout << glfwGetTime() - start << std::endl;
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

Block	&Chunk::getBlock(const mlm::ivec3 &blockChunkCoord)
{
	_blockMtx.lock();
	Block &ret = blocks[index3D(blockChunkCoord)];
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

mlm::ivec2							Chunk::getChunkPos()
{
	return (_chunkPos);
}

mlm::ivec3							Chunk::getWorldPos()
{
	return (_worldPos);
}

void								Chunk::setState(const Chunk::State state)
{
	_stateMtx.lock();
	_state = state;
	_stateMtx.unlock();
}

Chunk::State						Chunk::getState()
{
	_stateMtx.lock();
	State ret = _state;
	_stateMtx.unlock();
	return (ret);
}
