/*
Created by: Emily (Em_iIy) Winnink
Created on: 27/01/2026
*/

#include "Settings.hpp"

static void	loadVec4(mlm::vec4 &target, JSON::NodePtr node)
{
	JSON::ListPtr			array = node->getList();
	std::size_t				len = array->size();

	if (len != 4)
		throw std::runtime_error("color must be 4 numbers long");
	for (std::size_t i = 0; i < len; i++)
	{
		target[i] = (*array)[i]->getNumber();
	}
}

static void	loadGradient(GradientDTO &target, JSON::NodePtr node)
{
	loadVec4(target.sunriseCol, node->get("sunriseCol"));
	loadVec4(target.dayLowCol, node->get("dayLowCol"));
	loadVec4(target.dayHighCol, node->get("dayHighCol"));
	loadVec4(target.sunsetCol, node->get("sunsetCol"));
	loadVec4(target.nightLowCol, node->get("nightLowCol"));
	loadVec4(target.nightHighCol, node->get("nightHighCol"));
}

static void	loadTimeSettings(SkyTimeSettings &target, JSON::NodePtr node)
{
	target.sunriseLen = node->get("sunriseLen")->getNumber();
	target.dayLen = node->get("dayLen")->getNumber();
	target.sunsetLen = node->get("sunsetLen")->getNumber();
	target.nightLen = node->get("nightLen")->getNumber();
}

SkyDTO		Settings::loadSky()
{
	SkyDTO	skyDTO;
	try
	{
		JSON::Parser	terrainGeneratorJSON(_paths.at("sky"));

		JSON::NodePtr	root = terrainGeneratorJSON.getRoot();

		loadTimeSettings(skyDTO.timeSettings, root->get("timeSettings"));

		loadGradient(skyDTO.stop0, root->get("gradientStop0"));
		loadGradient(skyDTO.stop1, root->get("gradientStop1"));
		loadGradient(skyDTO.stop2, root->get("gradientStop2"));
		loadGradient(skyDTO.stop3, root->get("gradientStop3"));
		return (skyDTO);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
}
