/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/03/2026
*/

#pragma once

#include <string>
#include <vector>
#include <mutex>

class Logger {
	public:
		enum class Level {
			INFO,
			LOG,
			ERROR
		};
		static Level		convertLevel(const std::string &levelStr);
		static void			setLevel(const Level level);
		static void			info(const std::string &msg);
		static void			log(const std::string &msg);
		static void			error(const std::string &msg);

	private:
		static void			_print(const Level level, const std::string &msg);
		static bool			_checkLevel(const Level level);
		static Level		_level;
		static std::mutex	_levelMtx;
		static std::mutex	_printMtx;
};
