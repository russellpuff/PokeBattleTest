// Program.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Benjamin Russell
// This project was made as CSCI201 
#include <iostream>
#include <SFML/Graphics.hpp>
#include "EventListener.h"
#include "PBTExceptions.h"

int main()
{
    bool errorOccured = true; // defaults to true, sets to false if application exits safely, less copied code that way.
    std::string error, e;
    int retval = 0;
    try {
        sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
        Events::EventListener(window); // When this returns, PokeBattleTest execution starts to end. 
        // If MonDesigner started this, it should return control back to MonDesigner.
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
    catch (std::bad_alloc dre) {
        e = dre.what();
        error = "Encountered a problem reading a .db file: " + e;
        retval = 103;
    }
    catch (ex::StatsConstructionException sce) {
        e = sce.what();
        error = "Encountered a problem assembling stats: " + e;
        retval = 104;
    }
    /*
    catch (...) {
        std::string error = "Encountered an unexpected problem with no details.";
        retval = 201;
    }
    */
    if (errorOccured) { Events::Log(error); }
    Events::WriteLogToFile();
    return retval;
}