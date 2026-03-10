/*
Created by: Emily (Em_iIy) Winnink
Created on: 27/08/2025
*/

#include "Atlas.hpp"
#include "Settings.hpp"

Atlas::Atlas()
{}

Atlas::~Atlas()
{}

bool	Atlas::load()
{
	AtlasDTO	atlasDto = Settings::loadAtlas();

	bmp_t	bmp = load_bmp(atlasDto.filename.c_str());
	if (!bmp.data)
		return (false);
	
	mlm::vec2	offsetFormat(0.0f);
	offsetFormat.x = atlasDto.pixelWidth / static_cast<float>(bmp.width);
	offsetFormat.y = atlasDto.pixelWidth / static_cast<float>(bmp.height);

	for (auto &[type, textureOffsetNames] : atlasDto.blockOffsets)
	{
		std::vector<mlm::vec2>	temp;

		for (const std::string &textureOffsetName : textureOffsetNames)
			temp.push_back(atlasDto.textureOffsets.at(textureOffsetName) * offsetFormat);
		
		_offsets.insert({type, temp});
	}

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

void	Atlas::bind()
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
