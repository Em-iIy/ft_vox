/*
Created by: Emily (Em_iIy) Winnink
Created on: 02/02/2026
*/

#pragma once

#include "glu/gl-utils.hpp"

#include <list>
#include <functional>
#include <sys/stat.h>

using ShaderInit = std::function<void()>;

struct ShaderSrc
{
	Shader		&shader;
	ShaderInit	init;
	const char	*vertexFileName;
	struct stat	vertexStats;
	const char	*fragmentFileName;
	struct stat	fragmentStats;

	ShaderSrc(Shader &s, ShaderInit i, const char *v, struct stat vs, const char *f, struct stat fs);
};

class ShaderManager
{
	public:
		ShaderManager();

		static void	loadShader(Shader &shader, const char *vertexFileName, const char *fragmentFileName, ShaderInit init = nullptr);
		static void	reloadShaders();
		static void	unloadShaders();

	private:
		static std::list<ShaderSrc>	_shaders;
};
