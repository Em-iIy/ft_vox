/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#include "Chunk.hpp"

#include "Perlin.hpp"

#include "Spline.hpp"

#include "VoxEngine.hpp"

int chunk_count = 0;

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
	chunk_count++;
}

Chunk::~Chunk()
{
	_mesh.del();
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
			return (false);
		return (true);
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
	Block		&block = blocks[index3D(ipos)];
	Atlas		&atlas = _manager.getEngine().getAtlas();
	const std::vector<mlm::vec2>	&offsets = atlas.getOffset(block.getType());
	const std::vector<mlm::vec2>	&uvCorners = atlas.getCorners();

	std::vector<Expected<Block *, int>> blockNeighbors;
	for (const mlm::ivec3 &neighbor : neighbors)
	{
		auto neighborBlock = _manager.getBlock(worldPos + neighbor);
		blockNeighbors.push_back(neighborBlock);
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

float octaves(mlm::vec3 pos, uint64_t depth, float in_step)
{
	Perlin perlin;
	perlin.setSeed(4);
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

float	continentalnessSpline(const float value)
{
	static Spline spline({
		mlm::vec2(-1.f, 200.0f),
		mlm::vec2(-0.8f, 45.0f),
		mlm::vec2(-0.2f, 45.0f),
		mlm::vec2(0.0f, 58.0f),
		mlm::vec2(0.1f, 65.0f),
		mlm::vec2(0.25f, 70.0f),
		mlm::vec2(0.45f, 130.0f),
		mlm::vec2(1.00f, 150.0f),
	});
	float ret = spline.evaluate(value);
	return (ret);
}

float temp(const mlm::ivec3 &pos)
{
	return ((octaves(static_cast<mlm::vec3>(pos) / 400.0f, 5, 2.0f)));
}

int	heightRand(const mlm::ivec3 &pos)
{
	int	ret = 0;
	ret += static_cast<int>(continentalnessSpline(temp(pos)));
	return (ret);
}

void	Chunk::generate()
{
	const int	seaLevel = 60;
	for (uint64_t x = 0; x < CHUNK_SIZE_X; ++x)
	{
		for (uint64_t z = 0; z < CHUNK_SIZE_Z; ++z)
		{
			mlm::ivec3	iPos(x, 0, z);
			int			tempYMax = heightRand(iPos + mlm::ivec3(CHUNK_SIZE_X * _chunkPos.x, 1, CHUNK_SIZE_Z * _chunkPos.y));
			for (uint64_t y = 0; y < CHUNK_SIZE_Y; ++y)
			{
				iPos.y = y;
				uint64_t	index = index3D(x, y, z);
				mlm::vec3	color;
				Block::Type type = Block::STONE;
				if (iPos.y > tempYMax)
				{
					type = iPos.y <= seaLevel ? Block::WATER : Block::AIR;
					blocks[index] = Block(type);
					blocks[index].setEnabled(type == Block::AIR ? false : true);
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
					blocks[index] = Block(type);
					blocks[index].setEnabled(true);
				}
			}
		}
	}
	_setup = true;
}

void	Chunk::draw(Shader &shader)
{
	mlm::mat4 model(1.0f);
	model = mlm::translate(model, static_cast<mlm::vec3>(_worldPos) - _manager.getEngine().getCamera().getPos());
	shader.set_mat4("model", model);
	_mesh.draw(shader);
}

void	Chunk::update()
{
	std::vector<Vertex> vertices;
	vertices.reserve(8 * blocks.size());
	for (uint64_t x = 0; x < CHUNK_SIZE_X; ++x)
	{
		for (uint64_t y = 0; y < CHUNK_SIZE_Y; ++y)
		{
			for (uint64_t z = 0; z < CHUNK_SIZE_Z; ++z)
			{
				uint64_t	index = index3D(x, y, z);
				mlm::ivec3	pos(x, y, z);
				Block		&block = blocks[index];
				if (block.getEnabled())
					addCube(vertices, pos);
			}
		}
	}
	_mesh = ChunkMesh(vertices);
	_built = true;
}

Block	&Chunk::getBlock(const mlm::ivec3 &blockChunkCoord)
{
	return (blocks[index3D(blockChunkCoord)]);
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

bool	Chunk::isLoaded() const
{
	return (_loaded);
}

bool	Chunk::isSetup() const
{
	return (_setup);
}

bool	Chunk::isBuilt() const
{
	return (_built);
}

void	Chunk::requireRebuild()
{
	_built = false;
}
