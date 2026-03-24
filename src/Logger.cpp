/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/03/2026
*/

#include "Logger.hpp"

Logger::Level	Logger::_level = Logger::Level::LOG;
std::mutex		Logger::_mtx;

void	Logger::setLevel(const Logger::Level &level)
{
	_mtx.lock();
	_level = level;
	_mtx.unlock();
}
