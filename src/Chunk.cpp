/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#include "Chunk.hpp"

#include "Perlin.hpp"

Chunk::Chunk()
{
	
}

Chunk::Chunk(const mlm::ivec2 &chunkPos): _chunkPos(chunkPos)
{
}

Chunk::~Chunk()
{
}

static uint64_t	index3D(uint64_t x, uint64_t y, uint64_t z)
{
	return (z * (CHUNK_SIZE_Y * CHUNK_SIZE_X) + y * CHUNK_SIZE_X + x);
}

void	Chunk::addCube(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, const mlm::ivec3 &ipos)
{
	uint32_t	offset = vertices.size();
	mlm::vec3	pos(ipos);
	Block		&block = blocks[index3D(ipos.x, ipos.y, ipos.z)];
	mlm::vec3	color = block.getTypeColor();
	

	vertices.push_back({mlm::vec3(-0.5f, -0.5f, -0.5f) + pos, color * 0.7f, mlm::vec2(0.0f)}); // 0 back bottom left
	vertices.push_back({mlm::vec3(0.5f, -0.5f, -0.5f) + pos, color * 0.7f, mlm::vec2(0.0f)}); //  1 back bottom right
	vertices.push_back({mlm::vec3(-0.5f, 0.5f, -0.5f) + pos, color * 0.9f, mlm::vec2(0.0f)}); //  2 back top left
	vertices.push_back({mlm::vec3(0.5f, 0.5f, -0.5f) + pos, color * 0.9f, mlm::vec2(0.0f)}); //   3 back top right
	vertices.push_back({mlm::vec3(-0.5f, -0.5f, 0.5f) + pos, color * 0.7f, mlm::vec2(0.0f)}); //  4 front bottom left
	vertices.push_back({mlm::vec3(0.5f, -0.5f, 0.5f) + pos, color * 0.7f, mlm::vec2(0.0f)}); //   5 front bottom right
	vertices.push_back({mlm::vec3(-0.5f, 0.5f, 0.5f) + pos, color * 0.8f, mlm::vec2(0.0f)}); //   6 front top left
	vertices.push_back({mlm::vec3(0.5f, 0.5f, 0.5f) + pos, color * 0.8f, mlm::vec2(0.0f)}); //    7 front top right
	// back face
	if (ipos.z == 0 || blocks[index3D(ipos.x, ipos.y, ipos.z - 1)].getEnabled() == false)
	{
		indices.push_back(0 + offset);
		indices.push_back(2 + offset);
		indices.push_back(1 + offset);
		indices.push_back(1 + offset);
		indices.push_back(2 + offset);
		indices.push_back(3 + offset);
	}
	// front face
	if (ipos.z == CHUNK_SIZE_Z - 1 || blocks[index3D(ipos.x, ipos.y, ipos.z + 1)].getEnabled() == false)
	{
		indices.push_back(4 + offset);
		indices.push_back(5 + offset);
		indices.push_back(6 + offset);
		indices.push_back(5 + offset);
		indices.push_back(7 + offset);
		indices.push_back(6 + offset);
	}
	// left face
	if (ipos.x == 0 || blocks[index3D(ipos.x - 1, ipos.y, ipos.z)].getEnabled() == false)
	{
		indices.push_back(0 + offset);
		indices.push_back(4 + offset);
		indices.push_back(6 + offset);
		indices.push_back(0 + offset);
		indices.push_back(6 + offset);
		indices.push_back(2 + offset);
	}
	// right face
	if (ipos.x == CHUNK_SIZE_X - 1 || blocks[index3D(ipos.x + 1, ipos.y, ipos.z)].getEnabled() == false)
	{
		indices.push_back(1 + offset);
		indices.push_back(7 + offset);
		indices.push_back(5 + offset);
		indices.push_back(1 + offset);
		indices.push_back(3 + offset);
		indices.push_back(7 + offset);
	}
	// top face
	if (ipos.y == CHUNK_SIZE_Y - 1 || blocks[index3D(ipos.x, ipos.y + 1, ipos.z)].getEnabled() == false)
	{
		indices.push_back(2 + offset);
		indices.push_back(6 + offset);
		indices.push_back(7 + offset);
		indices.push_back(2 + offset);
		indices.push_back(7 + offset);
		indices.push_back(3 + offset);
	}
	// bottom face
	if (ipos.y == 0 || blocks[index3D(ipos.x, ipos.y - 1, ipos.z)].getEnabled() == false)
	{
		indices.push_back(1 + offset);
		indices.push_back(4 + offset);
		indices.push_back(0 + offset);
		indices.push_back(1 + offset);
		indices.push_back(5 + offset);
		indices.push_back(4 + offset);
	}
}

float octaves(mlm::vec3 pos, uint64_t depth, float step)
{
	Perlin perlin;
	perlin.setSeed(3);
	float ret = 0.0f;
	for (; depth > 0; --depth)
	{
		float temp = perlin.getValue(pos.x * step, pos.z * step) / step;
		if (std::abs(temp) > std::numeric_limits<float>::epsilon())
			ret += temp;
		else
			break ;
		step *= step;
	}
	return (ret);
}

int	heightRand(const mlm::ivec3 &pos)
{
	int	ret = 0;
	ret += static_cast<int>(octaves(static_cast<mlm::vec3>(pos) / 200.0f, 4, 2.0f) * 48.0f);
	return (ret);
}

void	Chunk::generate()
{
	const uint64_t	yMax = CHUNK_SIZE_Y / 2;
	for (uint64_t x = 0; x < CHUNK_SIZE_X; ++x)
	{
		for (uint64_t z = 0; z < CHUNK_SIZE_Z; ++z)
		{
			mlm::ivec3	iPos(x, 0, z);
			int			tempYMax = yMax + heightRand(iPos + mlm::ivec3(CHUNK_SIZE_X * _chunkPos.x, 1, CHUNK_SIZE_Z * _chunkPos.y));
			for (uint64_t y = 0; y < CHUNK_SIZE_Y; ++y)
			{
				iPos.y = y;
				uint64_t	index = index3D(x, y, z);
				mlm::vec3	color;
				if (iPos.y > tempYMax)
				{
					blocks[index] = Block(mlm::vec3(0.0f), Block::AIR);
					blocks[index].setEnabled(false);
				}
				else
				{
					Block::Type type = Block::STONE;
					if (iPos.y == tempYMax)
						type = Block::GRASS;
					else if (iPos.y < tempYMax && iPos.y > tempYMax - 4)
						type = Block::DIRT;
					blocks[index] = Block(mlm::vec3(1.0f), type);
					blocks[index].setEnabled(true);
				}
			}
		}
	}
	update();
}

void	Chunk::draw(Shader &shader)
{
	mlm::mat4 model(1.0f);
	model = mlm::translate(model, mlm::ivec3(CHUNK_SIZE_X * _chunkPos.x, 1, CHUNK_SIZE_Z * _chunkPos.y));
	shader.set_mat4("model", model);
	_mesh.draw(shader); // fix mesh (either add a single cube to test, or generate the whole chunk)
}

void	Chunk::update()
{
	std::vector<Vertex> vertices;
	vertices.reserve(8 * blocks.size());
	std::vector<uint32_t> indices;
	indices.reserve(6 * 2 * 3 * blocks.size()); // 6 Sides per cube, 2 triangles per side, 3 indices per triangle
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
					addCube(vertices, indices, pos);
			}
		}
	}
	_mesh = Mesh(vertices, indices);
}
