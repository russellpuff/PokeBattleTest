#pragma once
#include <SFML/Graphics.hpp>
#include <queue>

namespace Events {
	void Log(std::string& msg, std::queue<std::string>& log);
	void WriteLogToFile(std::queue<std::string> log);
	void EventListener(sf::RenderWindow& window, std::queue<std::string>& log);
}