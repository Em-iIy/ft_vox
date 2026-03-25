/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"
#include "Settings.hpp"
#include <iostream>

#include "Logger.hpp"

int main(int argc, char **argv)
{
	VoxEngine	engine;

	try
	{
		Settings::loadPaths(argc, argv);
		engine.run();
	}
	catch(const std::exception& e)
	{
		Logger::error(e.what());
		return (1);
	}
	return (0);
}
