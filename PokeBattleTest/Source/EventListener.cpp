#include <iostream>
#include <fstream>
#include <future>
#include <queue>
#include "EventListener.h"
#include "SceneConstructor.h"
#include "Pokemon.h"
#include "date.h"
#include "UUID.h"
#include "json.hpp"
#include "Battle.h"
std::queue<std::string> Events::log;
int TryParseInt(std::string s);

void Events::WriteToScreen(std::string& message)
{
    Log(message);
}

void Events::Log(std::string& msg) {
    std::string time = date::format("%F %R", std::chrono::system_clock::now());
    std::string logtext = "@" + time + ": " + msg;
    std::cout << logtext << std::endl;
    Events::log.push(logtext);
}

void Events::WriteLogToFile() { 
    std::string filename = "logs\\" + date::format("%F", std::chrono::system_clock::now()) + "-" + uuid::generate_uuid_v4() + ".txt";
    std::ofstream out(filename);
    while (!Events::log.empty()) {
        out << Events::log.front() << std::endl;
        Events::log.pop();
    }
    out.close();
}

void Events::EventListener(sf::RenderWindow& window) {
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    int x = 0, y = 0; // mouse cursor position

    std::tuple<pkmn::Pokemon, pkmn::Pokemon> pokemon = sc::PrepareAndConstructPokemon();
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
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    // do stuff based on where the button click was detected
                }
                break;

            case sf::Event::Closed:
                msg = "The window was closed.";
                Log(msg);
                window.close();
                break;

            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}

void Events::ConsoleBattleControl() // This method allows for controlling the battle through the console alone without a UI. 
{ // It was hastily put together to provide SOMETHING to test the incomplete application for the project submission. 
    std::tuple<pkmn::Pokemon, pkmn::Pokemon> pokemon = sc::PrepareAndConstructPokemon();
    bat::Battle battle(std::get<0>(pokemon), std::get<1>(pokemon));

    std::string uin;
    int p_m, r_m;
    
    while (1) {
        std::cout << std::get<0>(pokemon).GetName() << " has " << std::to_string(std::get<0>(pokemon).GetCurrentHP()) << "hp. " <<
            "Select a move by typing the number based on the earlier log output.\n";
        while (1) {
            std::getline(std::cin, uin);
            p_m = TryParseInt(uin);
            if (p_m > 4 || p_m < 1) { std::cout << "Invalid. Try again.\n"; }
            else { break; }
        }

        std::cout << std::get<1>(pokemon).GetName() << " has " << std::to_string(std::get<1>(pokemon).GetCurrentHP()) << "hp. " <<
            "Select a move by typing the number based on the earlier log output.\n";
        while (1) {
            std::getline(std::cin, uin);
            r_m = TryParseInt(uin);
            if (r_m > 4 || r_m < 1) { std::cout << "Invalid. Try again.\n"; }
            else { break; }
        }

        pkmn::Move p_move = std::get<0>(pokemon).GetMove(p_m);
        pkmn::Move r_move = std::get<1>(pokemon).GetMove(r_m);
        battle.Round(p_move, r_move);
        if (battle.GetVictorDeclared()) {
            std::string who = battle.GetLoserName() + " fainted!";
            Events::WriteToScreen(who);
            return;
        }
    }
}

int TryParseInt(std::string s) {
    int ret = -1;
    try { ret = stoi(s); } catch (...) {}
    return ret;
}
