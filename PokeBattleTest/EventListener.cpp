#include <iostream>
#include <future>
#include "EventListener.h"
#include "date.h"

void Events::Log(std::string msg) {
    std::string time = date::format("%F %T", std::chrono::system_clock::now());
    std::cout << "@" << time << ": " << msg << std::endl;
}

void Events::EventListener(sf::RenderWindow& window) {
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                std::string msg = "The window was closed.";
                auto disc = std::async(std::launch::async, Log, msg);
                window.close();
            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

}