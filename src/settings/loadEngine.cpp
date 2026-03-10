/*
Created by: Emily (Em_iIy) Winnink
Created on: 05/03/2026
*/

#include "Settings.hpp"

static void	loadCameraSettings(CameraSettings &target, JSON::NodePtr node)
{
	target.fov = node->get("fov")->getNumber();
	target.sensitivity = node->get("sensitivity")->getNumber();
	target.speed = node->get("speed")->getNumber();
	target.sprintMultiplier = node->get("sprintMultiplier")->getNumber();
}

static void	loadWindowSettings(WindowSettings &target, JSON::NodePtr node)
{
	target.width = node->get("width")->getNumber();
	target.height = node->get("height")->getNumber();
	target.fullscreen = node->get("fullscreen")->getBool();
	target.vsync = node->get("vsync")->getBool();
}

static void	validateSettings(const EngineDTO &engineDTO)
{
	if (engineDTO.cameraSettings.fov < 0.0f || engineDTO.cameraSettings.fov > 120.0f)
		throw std::runtime_error("Camera: fov must be between 0 and 120");
	if (engineDTO.windowSettings.width < 32.0f || engineDTO.windowSettings.height < 32.0f)
		throw std::runtime_error("Window: size must be larger than 32 pixels");
}

EngineDTO		Settings::loadEngine()
{
	EngineDTO		engineDTO;
	std::string	filename = {};
	try
	{
		filename = _paths.at("engine");
		JSON::Parser	terrainGeneratorJSON(filename);

		JSON::NodePtr	root = terrainGeneratorJSON.getRoot();

		loadCameraSettings(engineDTO.cameraSettings, root->get("camera"));
		loadWindowSettings(engineDTO.windowSettings, root->get("window"));

		validateSettings(engineDTO);
		return (engineDTO);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + filename + ": " + std::string(e.what()));
	}
}
