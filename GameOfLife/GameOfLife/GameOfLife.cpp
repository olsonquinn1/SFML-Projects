
#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>

int main()
{
    const int c_width = 1200;
    const int c_height = 960;
    const int c_gridSize = 15;
    int gridCount_x = c_width / c_gridSize;
    int gridCount_y = c_height / c_gridSize;

    float desiredFPS = 5.0;
    float frameTimeMS = 1000.0 / desiredFPS;

    sf::RenderWindow window(sf::VideoMode(c_width, c_height), "Title");
    window.setKeyRepeatEnabled(false);

    sf::RectangleShape cell;
    cell.setFillColor(sf::Color::Green);
    cell.setSize(sf::Vector2f(c_gridSize, c_gridSize) );

    sf::Font font;
    if (!font.loadFromFile("Roboto-Light.ttf")) {
        std::cout << "font error" << std::endl;
    }

    sf::Text pausedText("PAUSED", font, 30);
    pausedText.setFillColor(sf::Color(255, 0, 0, 100) );

    sf::Vertex line[2];

    sf::Event event;

    sf::Clock clock;

    srand(time(NULL));

    bool isPaused = false;

    bool mouseHeldDown = false;
    sf::Vector2i mouseLastPos;


    //generate 2d bool vector filled with false
    std::vector<std::vector<int>> prevState(gridCount_x);
    std::vector<int> column(gridCount_y);
    for (int i = 0; i < gridCount_y; i++)
        column[i] = 0;
    for (int i = 0; i < gridCount_x; i++) {
        prevState[i] = column;
    }
    column.clear();
    std::vector<std::vector<int>> newState = prevState;

    std::vector<int> yvals;
    for (int i = 0; i < c_height; i++)
        yvals.push_back(i);

    while (window.isOpen()) {

        while (window.pollEvent(event)) {

            if(event.type == sf::Event::Closed)
                window.close();

            //keyboard events
            if (event.type == sf::Event::KeyPressed) {

                //pause toggle
                if (event.key.code == sf::Keyboard::Space) {
                    isPaused = !isPaused;
                }

                //increase speed
                if (event.key.code == sf::Keyboard::Right) {
                    desiredFPS += 5;
                    frameTimeMS = 1000.0 / desiredFPS;
                    std::cout << "fps increase to " << desiredFPS << std::endl;
                }

                //decrease speed
                if (event.key.code == sf::Keyboard::Left) {
                    if (desiredFPS > 5) desiredFPS -= 5;
                    frameTimeMS = 1000.0 / desiredFPS;
                    std::cout << "fps decrease to " << desiredFPS << std::endl;
                }

                //randomize board state
                if (event.key.code == sf::Keyboard::R) {
                    for (int y = 0; y < gridCount_y; y++) {
                        for (int x = 0; x < gridCount_x; x++) {
                            if (rand() % 100 <= 50)
                                prevState[x][y] = 1;
                            else
                                prevState[x][y] = 0;
                        }
                    }
                }

                //clear board state
                if (event.key.code == sf::Keyboard::C) 
                    for (int y = 0; y < gridCount_y; y++) 
                        for (int x = 0; x < gridCount_x; x++) 
                            prevState[x][y] = 0;

            }

            //mouse events
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseHeldDown = true;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseLastPos = sf::Vector2i(-1, -1);
                    mouseHeldDown = false;
                }
            }

        }

        if (mouseHeldDown) {
            sf::Vector2i mousePos(sf::Mouse::getPosition(window) );
            mousePos.x = mousePos.x / c_gridSize;
            mousePos.y = mousePos.y / c_gridSize;
            if (mousePos != mouseLastPos && mousePos.x >= 0 && mousePos.x < gridCount_x && mousePos.y >= 0 && mousePos.y < gridCount_y)
                prevState[mousePos.x][mousePos.y] = (prevState[mousePos.x][mousePos.y] == 0 ? 1 : 0);
            mouseLastPos = mousePos;
        }

        if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {
            window.clear(sf::Color::Black);

            //update newState depending on prevState
            //iterate through each cell
            if (!isPaused) {
                newState = prevState;
                int aliveCount = 0;
                for (int y = 0; y < gridCount_y; y++)
                    for (int x = 0; x < gridCount_x; x++) {
                        aliveCount = 0;
                        //check surrounding cells
                        for (int dy = -1; dy <= 1; dy++) {

                            //outside boundary check
                            if (y + dy < 0 || y + dy > gridCount_y - 1)
                                continue;

                            for (int dx = -1; dx <= 1; dx++) {

                                //do not check self or outside boundary check
                                if (x + dx < 0 || x + dx > gridCount_x - 1 || (dx == 0 && dy == 0) )
                                    continue;

                                if (prevState[x + dx][y + dy] > 0)
                                    aliveCount++;
                            }
                        }

                        if (prevState[x][y] >= 1) { //cell was alive in prev state
                            if (aliveCount == 3)
                                newState[x][y] = 1;
                            else if (aliveCount == 2)
                                newState[x][y] = 1;
                            else
                                newState[x][y] = 0;
                        }
                        else { //cell was dead in prev state
                            if (aliveCount == 3)
                                newState[x][y] = 1;
                        }
                    }
                prevState = newState;
            }
            //draw prevState
            for (int y = 0; y < gridCount_y; y++)
                for (int x = 0; x < gridCount_x; x++)
                    if (prevState[x][y] > 0) {
                        if (prevState[x][y] > 1)
                            cell.setFillColor(sf::Color::Blue);
                        else
                            cell.setFillColor(sf::Color::Green);
                        cell.setPosition(sf::Vector2f(x * c_gridSize, y * c_gridSize));
                        window.draw(cell);
                    }

            //draw paused text
            if (isPaused)
                window.draw(pausedText);

            //draw grid lines
            for (int i = c_gridSize; i < c_width; i += c_gridSize) {
                line[0] = sf::Vertex(sf::Vector2f(i, 0), sf::Color::Color(64, 64, 64, 128));
                line[1] = sf::Vertex(sf::Vector2f(i, c_height), sf::Color::Color(64, 64, 64, 128));
                window.draw(line, 2, sf::Lines);
            }

            for (int i = c_gridSize; i < c_height; i += c_gridSize) {
                line[0] = sf::Vertex(sf::Vector2f(0, i), sf::Color::Color(64, 64, 64, 128));
                line[1] = sf::Vertex(sf::Vector2f(c_width, i), sf::Color::Color(64, 64, 64, 128));
                window.draw(line, 2, sf::Lines);
            }

            window.display();

            clock.restart();
        }
    }
}

