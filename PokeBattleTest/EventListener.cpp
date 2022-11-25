#include <iostream>
#include <fstream>
#include <future>
#include <queue>
#include "EventListener.h"
#include "date.h"
#include "UUID.h"

void Events::Log(std::string& msg, std::queue<std::string>& log) {
    std::string time = date::format("%F %R", std::chrono::system_clock::now());
    std::string logtext = "@" + time + ": " + msg;
    std::cout << logtext << std::endl;
    log.push(logtext);
}

void Events::WriteLogToFile(std::queue<std::string> log) { 
    std::string filename = "logs\\" + date::format("%F", std::chrono::system_clock::now()) + "-" + uuid::generate_uuid_v4() + ".txt";
    std::ofstream out(filename);
    while (!log.empty()) {
        out << log.front() << std::endl;
        log.pop();
    }
    out.close();
}

void Events::EventListener(sf::RenderWindow& window, std::queue<std::string>& log) {
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

    int x = 0, y = 0; // mouse cursor position
    


    while (window.isOpen())
    {
        std::string msg;
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
            case sf::Event::MouseMoved: // Track where the user's mouse is to detect "button" clicks.
                x = event.mouseMove.x;
                y = event.mouseMove.y;
                msg = "Detected mouse at " + std::to_string(x) + ", " + std::to_string(y);
                Log(msg, log);
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    // do stuff based on where the button click was detected
                }
                break;
            case sf::Event::Closed:
                msg = "The window was closed.";
                Log(msg, log);
                window.close();
                break;
            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

}