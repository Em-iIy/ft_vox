/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#include "Chunk.hpp"

#include "Perlin.hpp"

#include "Spline.hpp"

#include "VoxEngine.hpp"

int chunk_count = 0;

int	getChunkCount()
{
	return (chunk_count);
}

Chunk::Chunk(ChunkManager &manager): _manager(manager)
{
}

Chunk::Chunk(const mlm::ivec2 &chunkPos, ChunkManager &manager): _chunkPos(chunkPos), _manager(manager)
{
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

void	Chunk::addCube(std::vector<Vertex> &vertices, const mlm::ivec3 &ipos)
{
	mlm::ivec3	worldPos(_chunkPos.x * CHUNK_SIZE_X + ipos.x, ipos.y, _chunkPos.y * CHUNK_SIZE_Z + ipos.z);
	mlm::vec3	pos(ipos);
	Block		&block = blocks[index3D(ipos.x, ipos.y, ipos.z)];
	mlm::vec3	color = block.getTypeColor();
	const std::vector<mlm::vec2>	&offsets = _manager._engine._atlas.getOffset(block._type);
	
	const mlm::ivec3	neighbors[] = {
		mlm::ivec3(1, 0, 0),
		mlm::ivec3(-1, 0, 0),
		mlm::ivec3(0, 1, 0),
		mlm::ivec3(0, -1, 0),
		mlm::ivec3(0, 0, 1),
		mlm::ivec3(0, 0, -1),
	};
	const mlm::vec3	positions[] = {
		mlm::vec3(-0.5f, -0.5f, -0.5f) + pos, // 0 back bottom left
		mlm::vec3(0.5f, -0.5f, -0.5f) + pos, //  1 back bottom right
		mlm::vec3(-0.5f, 0.5f, -0.5f) + pos, //  2 back top left
		mlm::vec3(0.5f, 0.5f, -0.5f) + pos, //   3 back top right
		mlm::vec3(-0.5f, -0.5f, 0.5f) + pos, //  4 front bottom left
		mlm::vec3(0.5f, -0.5f, 0.5f) + pos, //   5 front bottom right
		mlm::vec3(-0.5f, 0.5f, 0.5f) + pos, //   6 front top left
		mlm::vec3(0.5f, 0.5f, 0.5f) + pos, //    7 front top right
	};
	// back face
	if (_manager.isBlockTransparent(worldPos + neighbors[5]) == false)
	{
		mlm::vec3	tempColor = color * 0.7f;
		vertices.push_back({positions[0], tempColor, offsets[1]});
		vertices.push_back({positions[2], tempColor, offsets[1]});
		vertices.push_back({positions[1], tempColor, offsets[1]});
		vertices.push_back({positions[1], tempColor, offsets[1]});
		vertices.push_back({positions[2], tempColor, offsets[1]});
		vertices.push_back({positions[3], tempColor, offsets[1]});
	}
	// front face
	if (_manager.isBlockTransparent(worldPos + neighbors[4]) == false)
	{
		mlm::vec3	tempColor = color * 0.7f;
		vertices.push_back({positions[4], tempColor, offsets[2]});
		vertices.push_back({positions[5], tempColor, offsets[2]});
		vertices.push_back({positions[6], tempColor, offsets[2]});
		vertices.push_back({positions[5], tempColor, offsets[2]});
		vertices.push_back({positions[7], tempColor, offsets[2]});
		vertices.push_back({positions[6], tempColor, offsets[2]});
	}
	// left face
	if (_manager.isBlockTransparent(worldPos + neighbors[1]) == false)
	{
		mlm::vec3	tempColor = color * 0.8f;
		vertices.push_back({positions[0], tempColor, offsets[3]});
		vertices.push_back({positions[4], tempColor, offsets[3]});
		vertices.push_back({positions[6], tempColor, offsets[3]});
		vertices.push_back({positions[0], tempColor, offsets[3]});
		vertices.push_back({positions[6], tempColor, offsets[3]});
		vertices.push_back({positions[2], tempColor, offsets[3]});
	}
	// right face
	if (_manager.isBlockTransparent(worldPos + neighbors[0]) == false)
	{
		mlm::vec3	tempColor = color * 0.8f;
		vertices.push_back({positions[1], tempColor, offsets[4]});
		vertices.push_back({positions[7], tempColor, offsets[4]});
		vertices.push_back({positions[5], tempColor, offsets[4]});
		vertices.push_back({positions[1], tempColor, offsets[4]});
		vertices.push_back({positions[3], tempColor, offsets[4]});
		vertices.push_back({positions[7], tempColor, offsets[4]});
	}
	// top face
	if (_manager.isBlockTransparent(worldPos + neighbors[2]) == false)
	{
		mlm::vec3	tempColor = color * 0.9f;
		vertices.push_back({positions[2], tempColor, offsets[0]});
		vertices.push_back({positions[6], tempColor, offsets[0]});
		vertices.push_back({positions[7], tempColor, offsets[0]});
		vertices.push_back({positions[2], tempColor, offsets[0]});
		vertices.push_back({positions[7], tempColor, offsets[0]});
		vertices.push_back({positions[3], tempColor, offsets[0]});
	}
	// bottom face
	if (_manager.isBlockTransparent(worldPos + neighbors[3]) == false)
	{
		mlm::vec3	tempColor = color * 0.9f;
		vertices.push_back({positions[1], tempColor, offsets[5]});
		vertices.push_back({positions[4], tempColor, offsets[5]});
		vertices.push_back({positions[0], tempColor, offsets[5]});
		vertices.push_back({positions[1], tempColor, offsets[5]});
		vertices.push_back({positions[5], tempColor, offsets[5]});
		vertices.push_back({positions[4], tempColor, offsets[5]});
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
	// static Spline spline({
	// 	mlm::vec2(-1.f, 50.0f),
	// 	mlm::vec2(0.3f, 100.0f),
	// 	mlm::vec2(0.4f, 150.0f),
	// 	mlm::vec2(1.0f, 150.0f),

	// });
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
				if (iPos.y > tempYMax)
				{
					blocks[index] = Block(Block::AIR);
					blocks[index].setEnabled(false);
				}
				else
				{
					Block::Type type = Block::STONE;
					if (iPos.y == tempYMax)
					{
						if (iPos.y < seaLevel)
							type = Block::WATER;
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
	mlm::vec3 pos = _manager._engine.getCamera().getPos();
	model = mlm::translate(model, static_cast<mlm::vec3>(mlm::ivec3(CHUNK_SIZE_X * _chunkPos.x, 0, CHUNK_SIZE_Z * _chunkPos.y)) - pos);
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
