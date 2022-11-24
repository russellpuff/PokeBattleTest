// Program.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Benjamin Russell
// This project was made as CSCI201 
#include <iostream>
#include <SFML/Graphics.hpp>
#include <queue>
#include "EventListener.h"
#include "PBTExceptions.h"
std::string what(const std::exception_ptr& eptr = std::current_exception());

int main()
{
    bool errorOccured = true; // defaults to true, sets to false if application exits safely, less copied code that way.
    std::string error, e;
    std::queue<std::string> log;
    int retval = 0;
    try {
        sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
        // When this returns, PokeBattleTest execution ends. If MonDesigner started this, it should return control back to MonDesigner.
        Events::EventListener(window, log); 
        errorOccured = false;
    }
    catch (ex::PokemonConstructionException pce) {
        e = pce.what();
        error = "Encountered a problem constructing Pokemon: " + e;
        retval = 101;
    }
    catch (ex::MoveConstructionException mce) {
        e = mce.what();
        error = "Encounterd a problem constructing a move: " + e;
        retval = 102;
    }
    catch (ex::DatabaseReadException dre) {
        e = dre.what();
        error = "Encountered a problem reading a .db file: " + e;
        retval = 103;
    }
    catch (ex::StatsConstructionException sce) {
        e = sce.what();
        error = "Encountered a problem assembling stats: " + e;
        retval = 104;
    }
    catch (...) {
        std::string error = "Encountered an unexpected problem.";
        retval = 201;
    }
    if (errorOccured) { Events::Log(error, log); }
    Events::WriteLogToFile(log);
    return retval;
}