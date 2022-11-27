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
    std::string oMonFile = "resource\\o-pkmn.json";
    std::string pTemFile = "resource\\p-template.json";
    std::string oTemFile = "resource\\o-template.json";

    std::string line, d = "";
    std::ifstream pm(pMonFile);
    while (std::getline(pm, line)) { d += line; }
    nlohmann::json monData = nlohmann::json::parse(d);
    const jt::JsonPkmn pMon = nlohmann::adl_serializer<jt::JsonPkmn>::from_json(monData);
    pm.close();

    d = "";
    std::ifstream om(oMonFile);
    while (std::getline(om, line)) { d += line; }
    monData = nlohmann::json::parse(d);
    const jt::JsonPkmn oMon = nlohmann::adl_serializer<jt::JsonPkmn>::from_json(monData);
    om.close();

    d = "";
    std::ifstream pt(pTemFile);
    while (std::getline(pt, line)) { d += line; }
    monData = nlohmann::json::parse(d);
    const jt::JsonTemplate pTem = nlohmann::adl_serializer<jt::JsonTemplate>::from_json(monData);
    pt.close();

    d = "";
    std::ifstream ot(oTemFile);
    while (std::getline(ot, line)) { d += line; }
    monData = nlohmann::json::parse(d);
    const jt::JsonTemplate oTem = nlohmann::adl_serializer<jt::JsonTemplate>::from_json(monData);
    ot.close();

    std::array<mon::Move, 4> pMoves = { pMon.Move1, pMon.Move2, pMon.Move3, pMon.Move4 };
    std::array<mon::Move, 4> oMoves = { oMon.Move1, oMon.Move2, oMon.Move3, oMon.Move4 };

    mon::Pokemon playerPokemon(pMon, pTem, pMoves);
    mon::Pokemon opponentPokemon(oMon, oTem, oMoves);
    
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
    std::string oInfo = "Created " + opponentPokemon.GetName() + " for opponent.\n";
    oInfo += "With form: " + opponentPokemon.GetForm() + ", Type1: " + opponentPokemon.GetType1() + ", Type2: " + opponentPokemon.GetType2();
    oInfo += "\nWith move1: " + opponentPokemon.GetMove1().GetName() + ", Type: " + opponentPokemon.GetMove1().GetType();
    oInfo += "\nWith move2: " + opponentPokemon.GetMove2().GetName() + ", Type: " + opponentPokemon.GetMove2().GetType();
    oInfo += "\nWith move3: " + opponentPokemon.GetMove3().GetName() + ", Type: " + opponentPokemon.GetMove3().GetType();
    oInfo += "\nWith move4: " + opponentPokemon.GetMove4().GetName() + ", Type: " + opponentPokemon.GetMove4().GetType();
    oInfo += "\nLevel: " + std::to_string(opponentPokemon.GetLevel());
    oInfo += "\nHP current: " + std::to_string(opponentPokemon.GetCurrentHP());
    oInfo += "\nAtk current: " + std::to_string(opponentPokemon.GetCurrentAtk());
    oInfo += "\nDef current: " + std::to_string(opponentPokemon.GetCurrentDef());
    oInfo += "\nSpAtk current: " + std::to_string(opponentPokemon.GetCurrentSpAtk());
    oInfo += "\nSpDef current: " + std::to_string(opponentPokemon.GetCurrentSpDef());
    oInfo += "\nSpd current: " + std::to_string(opponentPokemon.GetCurrentSpd());
    Log(oInfo, log);

    return std::make_pair(playerPokemon, opponentPokemon);
}