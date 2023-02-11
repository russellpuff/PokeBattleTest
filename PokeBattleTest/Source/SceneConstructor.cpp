#include <string>
#include <fstream>
#include "json.hpp"
#include "Pokemon.h"
#include "EventListener.h"
#include "SceneConstructor.h"
void LogConstructionResults(pkmn::Pokemon player, pkmn::Pokemon rival);
enum sc::MoveRestriction {
    NoRestriction,
    MustUseThisMove,
    CantUseThisMove
};
std::array<sc::MoveRestriction, 4> p_restrictMoves = { sc::NoRestriction, sc::NoRestriction, sc::NoRestriction, sc::NoRestriction };
std::array<sc::MoveRestriction, 4> r_restrictMoves = { sc::NoRestriction, sc::NoRestriction, sc::NoRestriction, sc::NoRestriction };

std::tuple<pkmn::Pokemon, pkmn::Pokemon> sc::PrepareAndConstructPokemon() {
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

    std::array<pkmn::Move, 4> pMoves = { pMon.Move1, pMon.Move2, pMon.Move3, pMon.Move4 };
    std::array<pkmn::Move, 4> oMoves = { rMon.Move1, rMon.Move2, rMon.Move3, rMon.Move4 };

    pkmn::Pokemon playerPokemon(pMon, pTem, pMoves);
    pkmn::Pokemon rivalPokemon(rMon, rTem, oMoves);

    LogConstructionResults(playerPokemon, rivalPokemon);
    return std::make_pair(playerPokemon, rivalPokemon);
}

void LogConstructionResults(pkmn::Pokemon player, pkmn::Pokemon rival) {
    std::string pInfo = "Created " + player.GetName() + " for player.";
    pInfo += "\nWith form: " + player.GetForm() + ", Type1: " + tc::typeToString.at(player.GetType1()) + 
        ", Type2: " + tc::typeToString.at(player.GetType2());
    pInfo += "\nWith move1: " + player.GetMove(1).GetName() + ", Type: " + tc::typeToString.at(player.GetMove(1).GetType());
    pInfo += "\nWith move2: " + player.GetMove(2).GetName() + ", Type: " + tc::typeToString.at(player.GetMove(2).GetType());
    pInfo += "\nWith move3: " + player.GetMove(3).GetName() + ", Type: " + tc::typeToString.at(player.GetMove(3).GetType());
    pInfo += "\nWith move4: " + player.GetMove(4).GetName() + ", Type: " + tc::typeToString.at(player.GetMove(4).GetType());
    pInfo += "\nLevel: " + std::to_string(player.GetLevel());
    pInfo += "\nHP current: " + std::to_string(player.GetFinalHP());
    pInfo += "\nAtk current: " + std::to_string(player.GetFinalAtk());
    pInfo += "\nDef current: " + std::to_string(player.GetFinalDef());
    pInfo += "\nSpAtk current: " + std::to_string(player.GetFinalSpAtk());
    pInfo += "\nSpDef current: " + std::to_string(player.GetFinalSpDef());
    pInfo += "\nSpd current: " + std::to_string(player.GetFinalSpd());
    Events::Log(pInfo);
    std::string rInfo = "Created " + rival.GetName() + " for rival.";
    rInfo += "\nWith form: " + rival.GetForm() + ", Type1: " + tc::typeToString.at(rival.GetType1()) + ", Type2: " + tc::typeToString.at(rival.GetType2());
    rInfo += "\nWith move1: " + rival.GetMove(1).GetName() + ", Type: " + tc::typeToString.at(rival.GetMove(1).GetType());
    rInfo += "\nWith move2: " + rival.GetMove(2).GetName() + ", Type: " + tc::typeToString.at(rival.GetMove(2).GetType());
    rInfo += "\nWith move3: " + rival.GetMove(3).GetName() + ", Type: " + tc::typeToString.at(rival.GetMove(3).GetType());
    rInfo += "\nWith move4: " + rival.GetMove(4).GetName() + ", Type: " + tc::typeToString.at(rival.GetMove(4).GetType());
    rInfo += "\nLevel: " + std::to_string(rival.GetLevel());
    rInfo += "\nHP current: " + std::to_string(rival.GetFinalHP());
    rInfo += "\nAtk current: " + std::to_string(rival.GetFinalAtk());
    rInfo += "\nDef current: " + std::to_string(rival.GetFinalDef());
    rInfo += "\nSpAtk current: " + std::to_string(rival.GetFinalSpAtk());
    rInfo += "\nSpDef current: " + std::to_string(rival.GetFinalSpDef());
    rInfo += "\nSpd current: " + std::to_string(rival.GetFinalSpd());
    Events::Log(rInfo);
}