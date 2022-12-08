#pragma once
#include <SFML/Graphics.hpp>
#include <queue>
#include "Pokemon.h"

namespace Events {
	extern std::queue<std::string> log;
	void WriteToScreen(std::string& message);
	void Log(std::string& msg);
	void WriteLogToFile();
	void EventListener(sf::RenderWindow& window);
	std::tuple<mon::Pokemon, mon::Pokemon> PrepareAndConstructPokemon();
}