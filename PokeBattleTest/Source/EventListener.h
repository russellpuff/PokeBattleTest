#pragma once
#include <SFML/Graphics.hpp>
#include <queue>
#include "Pokemon.h"

namespace Events {
	void Log(std::string& msg, std::queue<std::string>& log);
	void WriteLogToFile(std::queue<std::string>& log);
	void EventListener(sf::RenderWindow& window, std::queue<std::string>& log);
	std::tuple<mon::Pokemon, mon::Pokemon> PrepareAndConstructPokemon(std::queue<std::string>& log);
}