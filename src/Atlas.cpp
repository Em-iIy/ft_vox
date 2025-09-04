/*
Created by: Emily (Em_iIy) Winnink
Created on: 27/08/2025
*/

#include "Atlas.hpp"

Atlas::Atlas()
{}

Atlas::~Atlas()
{}

bool	Atlas::load(const std::string &atlasFileName, uint32_t len)
{
	bmp_t	bmp = load_bmp(atlasFileName.c_str());
	if (!bmp.data)
		return (false);
	mlm::vec2	offsetFormat(0.0f);
	offsetFormat.x = static_cast<float>(len) / static_cast<float>(bmp.width);
	offsetFormat.y = static_cast<float>(len) / static_cast<float>(bmp.height);

	_offsets.insert({Block::AIR, {offsetFormat * mlm::vec2(7.0f, 7.0f), offsetFormat * mlm::vec2(7.0f, 7.0f), offsetFormat * mlm::vec2(7.0f, 7.0f), offsetFormat * mlm::vec2(7.0f, 7.0f), offsetFormat * mlm::vec2(7.0f, 7.0f), offsetFormat * mlm::vec2(7.0f, 7.0f)}});
	_offsets.insert({Block::DIRT, {offsetFormat * mlm::vec2(1.0f, 7.0f), offsetFormat * mlm::vec2(1.0f, 7.0f), offsetFormat * mlm::vec2(1.0f, 7.0f), offsetFormat * mlm::vec2(1.0f, 7.0f), offsetFormat * mlm::vec2(1.0f, 7.0f), offsetFormat * mlm::vec2(1.0f, 7.0f)}});
	_offsets.insert({Block::GRASS, {offsetFormat * mlm::vec2(0.0f, 7.0f), offsetFormat * mlm::vec2(0.0f, 6.0f), offsetFormat * mlm::vec2(0.0f, 6.0f), offsetFormat * mlm::vec2(0.0f, 6.0f), offsetFormat * mlm::vec2(0.0f, 6.0f), offsetFormat * mlm::vec2(1.0f, 7.0f)}});
	_offsets.insert({Block::STONE, {offsetFormat * mlm::vec2(2.0f, 7.0f), offsetFormat * mlm::vec2(2.0f, 7.0f), offsetFormat * mlm::vec2(2.0f, 7.0f), offsetFormat * mlm::vec2(2.0f, 7.0f), offsetFormat * mlm::vec2(2.0f, 7.0f), offsetFormat * mlm::vec2(2.0f, 7.0f)}});
	_offsets.insert({Block::WATER, {offsetFormat * mlm::vec2(3.0f, 7.0f), offsetFormat * mlm::vec2(3.0f, 7.0f), offsetFormat * mlm::vec2(3.0f, 7.0f), offsetFormat * mlm::vec2(3.0f, 7.0f), offsetFormat * mlm::vec2(3.0f, 7.0f), offsetFormat * mlm::vec2(3.0f, 7.0f)}});

	_uvCorners = {
		mlm::vec2(0.0f, 0.0f) * (offsetFormat - std::numeric_limits<float>::epsilon()), // bottom left
		mlm::vec2(1.0f, 0.0f) * (offsetFormat - std::numeric_limits<float>::epsilon()), // bottom right
		mlm::vec2(0.0f, 1.0f) * (offsetFormat - std::numeric_limits<float>::epsilon()), // top left
		mlm::vec2(1.0f, 1.0f) * (offsetFormat - std::numeric_limits<float>::epsilon()), // top right
	};

	_texture.load(bmp);
	free_bmp(bmp);
	return (true);
}

void							Atlas::bind()
{
	_texture.bind();
}

const std::vector<mlm::vec2>	&Atlas::getOffset(Block::Type blockType)
{
	return (_offsets[blockType]);
}

const std::vector<mlm::vec2>	&Atlas::getCorners()
{
	return (_uvCorners);
}

void	Atlas::del()
{
	_texture.del();
}
