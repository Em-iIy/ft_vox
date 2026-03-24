/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/03/2026
*/

#pragma once

#include <vector>
#include <mutex>

class Logger {
	public:
		enum class Level {
			VERBOSE,
			LOG,
			INFO,
			ERROR
		};
		static void			setLevel(const Level &level);
	private:
		static Level		_level;
		static std::mutex	_mtx;
};
