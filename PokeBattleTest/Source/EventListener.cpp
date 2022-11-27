#include <iostream>
#include <fstream>
#include <future>
#include <queue>
#include "EventListener.h"
#include "Pokemon.h"
#include "date.h"
#include "UUID.h"
#include "json.hpp"

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

    std::tuple<mon::Pokemon, mon::Pokemon> pokemon = PrepareAndConstructPokemon(log);
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
                //msg = "Detected mouse at " + std::to_string(x) + ", " + std::to_string(y);
                //Log(msg, log);
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

std::tuple<mon::Pokemon, mon::Pokemon> Events::PrepareAndConstructPokemon(std::queue<std::string>& log) {
    std::string pMonFile = "resource\\p-pkmn.json";
    std::string rMonFile = "resource\\r-pkmn.json";
    std::string pTemFile = "resource\\p-template.json";
    std::string rTemFile = "resource\\r-template.json";

    std::string line, d = "";
    std::ifstream pm(pMonFile);
    while (std::getline(pm, line)) { d += line; }
    nlohmann::json monData = nlohmann::json::parse(d);
    const jt::JsonPkmn pMon = nlohmann::adl_serializer<jt::JsonPkmn>::from_json(monData);
    pm.close();

    d = "";
    std::ifstream rm(rMonFile);
    while (std::getline(rm, line)) { d += line; }
    monData = nlohmann::json::parse(d);
    const jt::JsonPkmn rMon = nlohmann::adl_serializer<jt::JsonPkmn>::from_json(monData);
    rm.close();

    d = "";
    std::ifstream pt(pTemFile);
    while (std::getline(pt, line)) { d += line; }
    monData = nlohmann::json::parse(d);
    const jt::JsonTemplate pTem = nlohmann::adl_serializer<jt::JsonTemplate>::from_json(monData);
    pt.close();

    d = "";
    std::ifstream rt(rTemFile);
    while (std::getline(rt, line)) { d += line; }
    monData = nlohmann::json::parse(d);
    const jt::JsonTemplate rTem = nlohmann::adl_serializer<jt::JsonTemplate>::from_json(monData);
    rt.close();

    std::array<mon::Move, 4> pMoves = { pMon.Move1, pMon.Move2, pMon.Move3, pMon.Move4 };
    std::array<mon::Move, 4> oMoves = { rMon.Move1, rMon.Move2, rMon.Move3, rMon.Move4 };

    mon::Pokemon playerPokemon(pMon, pTem, pMoves);
    mon::Pokemon rivalPokemon(rMon, rTem, oMoves);
    
    // Log results
    std::string pInfo = "Created " + playerPokemon.GetName() + " for player.\n";
    pInfo += "With form: " + playerPokemon.GetForm() + ", Type1: " + playerPokemon.GetType1() + ", Type2: " + playerPokemon.GetType2();
    pInfo += "\nWith move1: " + playerPokemon.GetMove1().GetName() + ", Type: " + playerPokemon.GetMove1().GetType();
    pInfo += "\nWith move2: " + playerPokemon.GetMove2().GetName() + ", Type: " + playerPokemon.GetMove2().GetType();
    pInfo += "\nWith move3: " + playerPokemon.GetMove3().GetName() + ", Type: " + playerPokemon.GetMove3().GetType();
    pInfo += "\nWith move4: " + playerPokemon.GetMove4().GetName() + ", Type: " + playerPokemon.GetMove4().GetType();
    pInfo += "\nLevel: " + std::to_string(playerPokemon.GetLevel());
    pInfo += "\nHP current: " + std::to_string(playerPokemon.GetCurrentHP());
    pInfo += "\nAtk current: " + std::to_string(playerPokemon.GetCurrentAtk());
    pInfo += "\nDef current: " + std::to_string(playerPokemon.GetCurrentDef());
    pInfo += "\nSpAtk current: " + std::to_string(playerPokemon.GetCurrentSpAtk());
    pInfo += "\nSpDef current: " + std::to_string(playerPokemon.GetCurrentSpDef());
    pInfo += "\nSpd current: " + std::to_string(playerPokemon.GetCurrentSpd());
    Log(pInfo, log);
    std::string rInfo = "Created " + rivalPokemon.GetName() + " for opponent.\n";
    rInfo += "With form: " + rivalPokemon.GetForm() + ", Type1: " + rivalPokemon.GetType1() + ", Type2: " + rivalPokemon.GetType2();
    rInfo += "\nWith move1: " + rivalPokemon.GetMove1().GetName() + ", Type: " + rivalPokemon.GetMove1().GetType();
    rInfo += "\nWith move2: " + rivalPokemon.GetMove2().GetName() + ", Type: " + rivalPokemon.GetMove2().GetType();
    rInfo += "\nWith move3: " + rivalPokemon.GetMove3().GetName() + ", Type: " + rivalPokemon.GetMove3().GetType();
    rInfo += "\nWith move4: " + rivalPokemon.GetMove4().GetName() + ", Type: " + rivalPokemon.GetMove4().GetType();
    rInfo += "\nLevel: " + std::to_string(rivalPokemon.GetLevel());
    rInfo += "\nHP current: " + std::to_string(rivalPokemon.GetCurrentHP());
    rInfo += "\nAtk current: " + std::to_string(rivalPokemon.GetCurrentAtk());
    rInfo += "\nDef current: " + std::to_string(rivalPokemon.GetCurrentDef());
    rInfo += "\nSpAtk current: " + std::to_string(rivalPokemon.GetCurrentSpAtk());
    rInfo += "\nSpDef current: " + std::to_string(rivalPokemon.GetCurrentSpDef());
    rInfo += "\nSpd current: " + std::to_string(rivalPokemon.GetCurrentSpd());
    Log(rInfo, log);

    return std::make_pair(playerPokemon, rivalPokemon);
}