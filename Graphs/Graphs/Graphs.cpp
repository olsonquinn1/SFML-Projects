// Graphs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 960), "Graphs");
    std::cout << "Hello World!\n";

    sf::Event event;

    while (window.isOpen()) {

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::EventType::Closed)
                window.close();
        }

    }
}
