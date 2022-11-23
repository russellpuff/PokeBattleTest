// Program.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Benjamin Russell
// This project was made as CSCI201 
#include <iostream>
#include <SFML/Graphics.hpp>
#include "EventListener.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    Events::EventListener(window); // When this returns, PokeBattleTest execution ends. If MonDesigner started this, it should return control back to MonDesigner.
    return 0;
}
