
#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>
#include <chrono>
#include <thread>

//returns distance between two points
float distance(float x1, float y1, float x2, float y2) {
    return sqrt(abs(x2 - x1) * abs(x2 - x1) + abs(y2 - y1) * abs(y2 - y1));
};

//returns integer of next grid position in the positive direction from val
int roundToGridPos(float val, int gridSize) {
    return (int) val + gridSize - ((int)val % gridSize);
};

//returns integer of next grid pos in negative direction from val
int roundToGridNeg(float val, int gridSize) {
    return (int) val - ((int)val % gridSize);
};

int main()
{
    const int c_width = 600;
    const int c_height = 480;
    const int gridSize = 20;
    const int c_gridWidth = c_width / gridSize;
    const int c_gridHeight = c_height / gridSize;
    const float desiredFPS = 60.0;

    sf::RenderWindow window(sf::VideoMode(c_width, c_height), "Title");

    sf::Event event;

    sf::Clock clock;
    
    float frameTimeMS = 1000.0 / desiredFPS;

    sf::Vertex gridLine[2];

    sf::CircleShape start;
    start.setRadius(5);
    start.setFillColor(sf::Color::White);
    start.setOrigin(sf::Vector2f(start.getRadius(), start.getRadius()));
    sf::Vector2f startPos;

    sf::CircleShape end;
    end.setRadius(5);
    end.setFillColor(sf::Color::Green);
    end.setOrigin(sf::Vector2f(end.getRadius(), end.getRadius()));
    
    sf::Vector2f endPos(c_width / 2, c_height / 2);

    end.setPosition(endPos);

    sf::Vector2i direction;

    sf::Vertex line[2];

    std::vector<sf::Vector2f> intersections; //list of grid coords ray intersects

    sf::RectangleShape intersectTile;
    intersectTile.setFillColor(sf::Color(255, 255, 255, 100));
    intersectTile.setSize(sf::Vector2f(gridSize, gridSize));

    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    endPos = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    //do the raycast:
                    //determine axes directions
                    if (startPos.x > endPos.x)
                        direction.x = -1;
                    else if (startPos.x < endPos.x)
                        direction.x = 1;
                    else
                        direction.x = 0;

                    if (startPos.y > endPos.y)
                        direction.y = -1;
                    else if (startPos.y < endPos.y)
                        direction.y = 1;
                    else
                        direction.y = 0;

                    //check if start and end pos share the same x or y grid position
                    if (roundToGridNeg(startPos.x, gridSize) == roundToGridNeg(endPos.x, gridSize))
                        direction.x = 0;
                    if (roundToGridNeg(startPos.y, gridSize) == roundToGridNeg(endPos.y, gridSize))
                        direction.y = 0;

                    intersections.clear();

                    //line properties
                    float slope = (endPos.y - startPos.y) / (endPos.x - startPos.x);
                    float yOffset = startPos.y - (slope * startPos.x);

                    int xCurrent = 0;
                    int xEnd = 0;

                    int yCurrent = 0;
                    int yEnd = 0;

                    //determine which values of x and y to check for intercepts
                    if (direction.x != 0) {
                        if (direction.x > 0) {
                            xCurrent = roundToGridPos(startPos.x, gridSize);
                            xEnd = roundToGridNeg(endPos.x, gridSize);
                        }
                        else {
                            xCurrent = roundToGridNeg(startPos.x, gridSize);
                            xEnd = roundToGridPos(endPos.x, gridSize);
                        }
                    }

                    if (direction.y != 0) {
                        if (direction.y > 0) {
                            yCurrent = roundToGridPos(startPos.y, gridSize);
                            yEnd = roundToGridNeg(endPos.y, gridSize);
                        }
                        else {
                            yCurrent = roundToGridNeg(startPos.y, gridSize);
                            yEnd = roundToGridPos(endPos.y, gridSize);
                        }
                    }

                    //add tile startpos lies on to intersections
                    intersections.push_back(sf::Vector2f(
                        roundToGridNeg(startPos.x, gridSize),
                        roundToGridNeg(startPos.y, gridSize)
                    ));

                    //go through x and y values and add the corresponding tile to intersections, offset depending on direction
                    if (direction.x != 0) {
                        int end = abs(xEnd - xCurrent);
                        int offset = (direction.x < 0 ? gridSize : 0);
                        for (int i = 0; i <= end; i += gridSize) {
                            float yIntercept = (slope * xCurrent) + yOffset;
                            intersections.push_back(sf::Vector2f(
                                xCurrent - offset,
                                roundToGridNeg(yIntercept, gridSize)
                            ));
                            xCurrent += gridSize * direction.x;
                        }
                    }

                    if (direction.y != 0) {
                        int end = abs(yEnd - yCurrent);
                        int offset = (direction.y < 0 ? gridSize : 0);
                        for (int i = 0; i <= end; i += gridSize) {
                            float xIntercept = (yCurrent - yOffset) / slope;
                            intersections.push_back(sf::Vector2f(
                                roundToGridNeg(xIntercept, gridSize),
                                yCurrent - offset
                            ));
                            yCurrent += gridSize * direction.y;
                        }
                    }
                }
            }
        }

        if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {

            //update position marker
            startPos = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

            //update line
            line[0] = sf::Vertex(startPos, sf::Color::Blue);
            line[1] = sf::Vertex(endPos, sf::Color::Blue);

            window.clear();

            //render grid lines
            for (int i = 0; i < c_gridWidth; i++) {
                gridLine[0] = sf::Vertex(sf::Vector2f(i * gridSize, 0), sf::Color::Color(64, 64, 64, 128));
                gridLine[1] = sf::Vertex(sf::Vector2f(i * gridSize, c_height), sf::Color::Color(64, 64, 64, 128));
                window.draw(gridLine, 2, sf::Lines);
            }
            for (int i = 0; i < c_gridHeight; i++) {
                gridLine[0] = sf::Vertex(sf::Vector2f(0, i * gridSize), sf::Color::Color(64, 64, 64, 128));
                gridLine[1] = sf::Vertex(sf::Vector2f(c_width, i * gridSize), sf::Color::Color(64, 64, 64, 128));
                window.draw(gridLine, 2, sf::Lines);
            }

            //render intersected tiles
            for (std::vector<sf::Vector2f>::iterator it = intersections.begin(); it < intersections.end(); it++) {
                intersectTile.setPosition(*it);
                window.draw(intersectTile);
            }


            //render markers

            start.setPosition(startPos);
            window.draw(start);

            end.setPosition(endPos);
            window.draw(end);

            //render line
            window.draw(line, 2, sf::Lines);

            clock.restart();
            window.display();
        }
    }
}