/*
Created by: Emily (Em_iIy) Winnink
Created on: 02/02/2026
*/

#include "ShaderManager.hpp"
#include "Logger.hpp"

std::list<ShaderSrc>	ShaderManager::_shaders;

ShaderSrc::ShaderSrc(Shader &s, ShaderInit i, const char *v, struct stat vs, const char *f, struct stat fs):
	shader(s),
	init(i),
	vertexFileName(v),
	vertexStats(vs),
	fragmentFileName(f),
	fragmentStats(fs)
{};

ShaderManager::ShaderManager() {}

void	ShaderManager::loadShader(Shader &shader, const char *vertexFileName, const char *fragmentFileName, ShaderInit init)
{
	Logger::info("Loading shader: " + std::string(vertexFileName) + " " + std::string(fragmentFileName));
	try
	{
		// Create shader
		shader = Shader(vertexFileName, fragmentFileName);
	}
	catch(...)
	{
		Logger::error("In shader: " + std::string(vertexFileName) + " " + std::string(fragmentFileName));
	}
	// Fetch stats for the shader files
	struct stat	vs;
	struct stat	fs;
	stat(vertexFileName, &vs);
	stat(fragmentFileName, &fs);

	// Run init callback
	if (init != nullptr)
	{
		shader.use();
		init();
	}

	// Add shader with info to list
	_shaders.emplace_back(shader, init, vertexFileName, vs, fragmentFileName, fs);
}

void	ShaderManager::reloadShaders()
{
	for (ShaderSrc &src : _shaders)
	{
		// Fetch current stats for the shader files
		struct stat	vs;
		struct stat	fs;
		stat(src.vertexFileName, &vs);
		stat(src.fragmentFileName, &fs);

		// Only reload shader if either file has been modified more recently
		if (vs.st_mtim.tv_sec > src.vertexStats.st_mtim.tv_sec || fs.st_mtim.tv_sec > src.fragmentStats.st_mtim.tv_sec)
		{
			try
			{
				Logger::info("Reloading shader: " + std::string(src.vertexFileName) + " " + std::string(src.fragmentFileName));
				Shader temp = Shader(src.vertexFileName, src.fragmentFileName);
				src.shader.del();
				src.shader = temp;

				// Update stats
				src.vertexStats = vs;
				src.fragmentStats = fs;

				// Reinitialize shader with callback
				if (src.init != nullptr)
				{
					src.shader.use();
					src.init();
				}
			}
			catch(...)
			{
				Logger::error("In shader: " + std::string(src.vertexFileName) + " " + std::string(src.fragmentFileName));
			}
		}
	}
}

void	ShaderManager::unloadShaders()
{
	Logger::info("Unloading shaders");
	for (ShaderSrc &src : _shaders)
		src.shader.del();
}
