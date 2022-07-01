
#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>

//dir can be 0 - 3 (up, right, down, left), (change can be -1 or 1)
int changeDirection(int oldDir, int change) {
    int newDir = oldDir + (change * 1);
    if (newDir < 0) newDir = 3;
    else if (newDir > 3) newDir = 0;
    return newDir;
}


int main()
{
    const int c_width = 1600;
    const int c_height = 1280;
    const int c_gridSize = 4;
    int gridCount_x = c_width / c_gridSize;
    int gridCount_y = c_height / c_gridSize;

    float desiredFPS = 3000.0;
    float frameTimeMS = 1000.0 / desiredFPS;

    sf::RenderWindow window(sf::VideoMode(c_width, c_height), "Langton Ant");

    sf::RenderTexture renderTexture;
    if (!renderTexture.create(c_width, c_height) ) {
        std::cout << "texture error" << std::endl;
    }

    sf::Event event;

    sf::Clock clock;

    sf::Vertex line[2];

    sf::Font font;
    if (!font.loadFromFile("Roboto-Light.ttf")) {
        std::cout << "font error" << std::endl;
    }

    int step = 0;

    sf::Text stepText(std::to_string(step), font, 30);
    stepText.setFillColor(sf::Color(255, 0, 0, 100));

    std::vector<std::vector<int>> state(gridCount_x);
    std::vector<int> column(gridCount_y);
    for (int i = 0; i < gridCount_y; i++)
        column[i] = 0;
    for (int i = 0; i < gridCount_x; i++)
        state[i] = column;

    sf::RectangleShape cell;
    cell.setFillColor(sf::Color::White);
    cell.setSize(sf::Vector2f(c_gridSize, c_gridSize));


    sf::Vector2i antGridPosition(gridCount_x / 2, gridCount_y / 2);

    sf::CircleShape antShape;
    antShape.setRadius((float)c_gridSize / 4.0f);
    antShape.setOrigin(sf::Vector2f((float)c_gridSize / 4.0f * -1, (float)c_gridSize / 4.0f * -1));
    antShape.setFillColor(sf::Color::Black);

    int antDir = 0;

    int currentState;

    //bounds that expand as ant travels to avoid spending resources on drawing blank cells
    int xBoundLo = antGridPosition.x;
    int xBoundHi = antGridPosition.x + 1;
    int yBoundLo = antGridPosition.y;
    int yBoundHi = antGridPosition.y + 1;

    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
        }



        if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {

            window.clear(sf::Color::Color(177, 177, 177, 255));

            //interesting states
            //LLRLRLL triangle highway
            //

            //determine state of current tile and do stuff depending on state
            currentState = state[antGridPosition.x][antGridPosition.y];
            if (currentState == 0) {
                antDir = changeDirection(antDir, -1);
                state[antGridPosition.x][antGridPosition.y] = 1;
            }
            else if (currentState == 1) {
                antDir = changeDirection(antDir, -1);
                state[antGridPosition.x][antGridPosition.y] = 2;
            }
            else if (currentState == 2) {
                antDir = changeDirection(antDir, 1);
                state[antGridPosition.x][antGridPosition.y] = 3;
            }
            else if (currentState == 3) {
                antDir = changeDirection(antDir, -1);
                state[antGridPosition.x][antGridPosition.y] = 4;
            }
            else if (currentState == 4) {
                antDir = changeDirection(antDir, 1);
                state[antGridPosition.x][antGridPosition.y] = 5;
            }
            else if (currentState == 5) {
                antDir = changeDirection(antDir, -1);
                state[antGridPosition.x][antGridPosition.y] = 6;
            }
            else if (currentState == 6) {
                antDir = changeDirection(antDir, -1);
                state[antGridPosition.x][antGridPosition.y] = 0;
            }
            //else if (currentState == 7) {
            //    antDir = changeDirection(antDir, 1);
            //    state[antGridPosition.x][antGridPosition.y] = 8;
            //}
            //else if (currentState == 8) {
            //    antDir = changeDirection(antDir, -1);
            //    state[antGridPosition.x][antGridPosition.y] = 0;
            //}
            //else if (currentState == 9) {
            //    antDir = changeDirection(antDir, -1);
            //    state[antGridPosition.x][antGridPosition.y] = 10;
            //}
            //else if (currentState == 10) {
            //    antDir = changeDirection(antDir, -1);
            //    state[antGridPosition.x][antGridPosition.y] = 11;
            //}
            //else if (currentState == 11) {
            //    antDir = changeDirection(antDir, 1);
            //    state[antGridPosition.x][antGridPosition.y] = 0;
            //}

            if (currentState == 0)
                cell.setFillColor(sf::Color::White);
            else if (currentState == 1)
                cell.setFillColor(sf::Color::Color(255, 0, 0, 255));
            else if (currentState == 2)
                cell.setFillColor(sf::Color::Color(0, 255, 0, 255));
            else if (currentState == 3)
                cell.setFillColor(sf::Color::Color(0, 0, 255, 255));
            else if (currentState == 4)
                cell.setFillColor(sf::Color::Color(255, 255, 0, 255));
            else if (currentState == 5)
                cell.setFillColor(sf::Color::Color(255, 0, 255, 255));
            else if (currentState == 6)
                cell.setFillColor(sf::Color::Color(0, 255, 255, 255));
            else if (currentState == 7)
                cell.setFillColor(sf::Color::Color(255, 255, 255, 255));
            else if (currentState == 8)
                cell.setFillColor(sf::Color::Color(177, 0, 0, 255));
            else if (currentState == 9)
                cell.setFillColor(sf::Color::Color(0, 177, 0, 255));
            else if (currentState == 10)
                cell.setFillColor(sf::Color::Color(0, 0, 177, 255));
            else if (currentState == 11)
                cell.setFillColor(sf::Color::Color(177, 177, 0, 255));
            else if (currentState == 12)
                cell.setFillColor(sf::Color::Color(177, 0, 177, 255));
            cell.setPosition(sf::Vector2f(antGridPosition.x * c_gridSize, antGridPosition.y * c_gridSize));
            renderTexture.draw(cell);

            //move ant forward in new direction
            int xOffset = 0;
            int yOffset = 0;
            if (antDir == 2) {
                yOffset = 1;
                xOffset = 0;
            }

            else if (antDir == 0) {
                yOffset = -1;
                xOffset = 0;
            }

            if (antDir == 1) {
                xOffset = 1;
                yOffset = 0;
            }

            else if (antDir == 3) {
                xOffset = -1;
                yOffset = 0;
            }

            antGridPosition.x = antGridPosition.x + xOffset;
            antGridPosition.y = antGridPosition.y + yOffset;

            //dont let ant go out of boundaries
            if (antGridPosition.x >= gridCount_x)
                antGridPosition.x = gridCount_x - 1;
            if (antGridPosition.x < 0)
                antGridPosition.x = 0;
            if (antGridPosition.y >= gridCount_y)
                antGridPosition.y = gridCount_y - 1;
            if (antGridPosition.y < 0)
                antGridPosition.y = 0;

            //draw pixel map
            const sf::Texture& texture = renderTexture.getTexture();
            sf::Sprite sprite(texture);
            window.draw(sprite);
            

            //draw step text
            stepText.setString(std::to_string(step));
            step++;
            window.draw(stepText);

            clock.restart();
            window.display();
        }
        
    }
}



