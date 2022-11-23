#pragma once
#include <SFML/Graphics.hpp>

namespace Events {
	void Log(std::string msg);
	void EventListener(sf::RenderWindow& window);
}