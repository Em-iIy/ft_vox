/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/03/2026
*/

#include "Logger.hpp"

#include <iostream>
#include <unordered_map>

#define RESET "\033[m"
#define RED "\033[38;5;1m"
#define GREEN "\033[38;5;2m"
#define YELLOW "\033[38;5;3m"
#define BLUE "\033[38;5;4m"
#define PURPLE "\033[38;5;5m"
#define ORANGE "\033[38;5;208m"
#define PINK "\033[38;5;200m"

const std::unordered_map<Logger::Level, std::string>	levelToString = {
	{Logger::Level::INFO, BLUE"[INFO]"},
	{Logger::Level::LOG, GREEN"[LOG]"},
	{Logger::Level::ERROR, RED"[ERROR]"},
};

const std::unordered_map<std::string, Logger::Level>	stringToLevel = {
	{"INFO", Logger::Level::INFO},
	{"LOG", Logger::Level::LOG},
	{"ERROR", Logger::Level::ERROR},
};

Logger::Level	Logger::_level = Logger::Level::LOG;
std::mutex		Logger::_levelMtx;
std::mutex		Logger::_printMtx;

Logger::Level	Logger::convertLevel(const std::string &levelStr)
{
	try
	{
		return (stringToLevel.at(levelStr));
	}
	catch(...)
	{
		throw std::runtime_error("Logger: `" + levelStr + "` Invalid logging level");
	}
}

void	Logger::setLevel(const Logger::Level level)
{
	_levelMtx.lock();
	_level = level;
	_levelMtx.unlock();
}

void	Logger::info(const std::string &msg)
{
	_print(Level::INFO, msg);
}

void	Logger::log(const std::string &msg)
{
	_print(Level::LOG, msg);
}

void	Logger::error(const std::string &msg)
{
	_print(Level::ERROR, msg);
}

void	Logger::_print(const Level level, const std::string &msg)
{
	if (!_checkLevel(level))
		return ;
	const std::string &levelStr = levelToString.at(level);
	_printMtx.lock();
	std::cout << levelStr << "\t- " << msg << RESET << std::endl;
	_printMtx.unlock();
}

bool	Logger::_checkLevel(const Level level)
{
	_levelMtx.lock();
	bool ret = (level >= _level);
	_levelMtx.unlock();
	return (ret);
}
