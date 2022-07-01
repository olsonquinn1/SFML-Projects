// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>

class SnakeSegment {

private:

    bool newBorn;
    int id;
    float gridSize;

    sf::RectangleShape rect;
    sf::Vector2f position;

public:


    SnakeSegment(sf::Vector2f position, float gridSize, int id) {

        this->gridSize = gridSize;
        this->position = position;
        this->id = id;
        
        newBorn = true;

        rect.setFillColor(sf::Color::White);
        rect.setSize(sf::Vector2f(gridSize, gridSize));
        rect.setPosition(position);
        
    }

    void setNewBorn(bool newBorn) {

        this->newBorn = newBorn;

    }

    int getId() {
        return id;
    }

    //update location (not on first frame alive)
    void update(sf::Vector2f newPos) {

        if (newBorn) {
            newBorn = false;
        }

        else {
            position = newPos;
            rect.setPosition(newPos);
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(rect);

    }

    sf::Vector2f getPos() {

        return position;

    }
};

int main(int argc, char** argv)
{
    srand(time(NULL));

    const float WIDTH = 600;
    const float HEIGHT = 480;
    const float GRIDSIZE = 40;
    int gridAmountX = WIDTH / GRIDSIZE;
    int gridAmountY = HEIGHT / GRIDSIZE;
    const float FPS = 5.0f;

    float frameTimeMS = 1000.0f / FPS;

    std::vector<std::vector<bool>> snakeLocation(gridAmountX);
    std::vector<bool> column(gridAmountY);
    for (int i = 0; i < gridAmountY; i++)
        column[i] = false;
    for (int i = 0; i < gridAmountX; i++)
        snakeLocation[i] = column;
    column.clear();

    

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Snake");
    window.setKeyRepeatEnabled(false);

    sf::Event event;

    sf::Clock clock;

    int dirLast = 1;
    int dir = 1; //0: up, 1: right, 2:down, 3:left
    int xOffset = 0;
    int yOffset = 0;
    sf::Vector2f newPosition;

    sf::Vertex line[2];

    std::vector<SnakeSegment> segments;
    segments.push_back(SnakeSegment(sf::Vector2f(120, 120), GRIDSIZE, 1) );
    segments.front().setNewBorn(false);

    snakeLocation[segments.front().getPos().x / GRIDSIZE][segments.front().getPos().y / GRIDSIZE] = true;

    bool addSegment = false;

    sf::CircleShape food;
    food.setFillColor(sf::Color::Red);
    food.setRadius(GRIDSIZE / 2);
    bool foodExists = false;
    

    while (window.isOpen()) {
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::EventType::Closed)
                window.close();

            //key pressed events
            else if (event.type = sf::Event::EventType::KeyPressed) {

                if (event.key.code == sf::Keyboard::Up && dirLast != 2)
                    dir = 0;

                else if (event.key.code == sf::Keyboard::Right && dirLast != 3)
                    dir = 1;

                else if (event.key.code == sf::Keyboard::Down && dirLast != 0)
                    dir = 2;

                else if (event.key.code == sf::Keyboard::Left && dirLast != 1)
                    dir = 3;

            }
        }

        if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {
            window.clear(sf::Color::Black);

            //render grid lines
            for (int i = GRIDSIZE; i < WIDTH; i += GRIDSIZE) {
                line[0] = sf::Vertex(sf::Vector2f(i, 0), sf::Color::Color(64, 64, 64, 128));
                line[1] = sf::Vertex(sf::Vector2f(i, HEIGHT), sf::Color::Color(64, 64, 64, 128));
                window.draw(line, 2, sf::Lines);
            }

            for (int i = GRIDSIZE; i < HEIGHT; i += GRIDSIZE) {
                line[0] = sf::Vertex(sf::Vector2f(0, i), sf::Color::Color(64, 64, 64, 128));
                line[1] = sf::Vertex(sf::Vector2f(WIDTH, i), sf::Color::Color(64, 64, 64, 128));
                window.draw(line, 2, sf::Lines);
            } 

            //food gen
            if (!foodExists) {
                sf::Vector2f foodPos(sf::Vector2f(((rand() % (int)(WIDTH / GRIDSIZE)) * GRIDSIZE), (rand() % (int)(HEIGHT / GRIDSIZE)) * GRIDSIZE) );
                food.setPosition(foodPos);
                foodExists = true;
            }

            //determine new position based on input
            dirLast = dir;
            if (dir == 2) {
                yOffset = 1;
                xOffset = 0;
            }

            else if (dir == 0) {
                yOffset = -1;
                xOffset = 0;
            }

            if (dir == 1) {
                xOffset = 1;
                yOffset = 0;
            }

            else if (dir == 3) {
                xOffset = -1;
                yOffset = 0;
            }
            newPosition.x = segments.front().getPos().x + (xOffset * GRIDSIZE);
            newPosition.y = segments.front().getPos().y + (yOffset * GRIDSIZE);
            if (newPosition.x < 0) newPosition.x = 600 - GRIDSIZE;
            else if (newPosition.x >= 600) newPosition.x = 0;

            if (newPosition.y < 0) newPosition.y = 480 - GRIDSIZE;
            else if (newPosition.y >= 480) newPosition.y = 0;

            //self collision check
            //first mark tail section as free (if no new segments being created)
            if (!addSegment) 
                snakeLocation[segments.back().getPos().x / GRIDSIZE][segments.back().getPos().y / GRIDSIZE] = false;

            //second check if new pos is not free
            if (snakeLocation[newPosition.x / GRIDSIZE][newPosition.y / GRIDSIZE]) {
                //lose
                window.close();
            }
            //if you dont lose, continue the game
            else {
                //third mark new pos as not free if size > 1
                if(segments.size() > 1) snakeLocation[segments.front().getPos().x / GRIDSIZE][segments.front().getPos().y / GRIDSIZE] = true;

                //add segment
                if (addSegment) {
                    segments.push_back(SnakeSegment(segments.back().getPos(), GRIDSIZE, segments.back().getId() + 1));
                    addSegment = false;
                }

                //update segments from back to front
                for (int i = segments.size() - 1; i > 0; i--) {
                    segments[i].update(segments[i - 1].getPos());
                    segments[i].draw(window);
                }

                //update first segment with new position
                segments[0].update(newPosition);
                segments[0].draw(window);

                //test food collision
                if (segments[0].getPos() == food.getPosition()) {
                    addSegment = true;
                    foodExists = false;
                }

                if (foodExists)
                    window.draw(food);

                clock.restart();
                window.display();
            }
            
        }
    }
}

