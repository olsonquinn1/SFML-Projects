// BouncyBalls.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>

class Ball {
private:
    sf::Vector2f velocity;
    sf::Vector2f position;

    sf::CircleShape circle;

    int id;
    int boundHeight;
    int boundWidth;

public:
    Ball(float radius, sf::Vector2f initialPos, sf::Vector2f initialVel, int boundHeight, int boundWidth, int id) {
        
        position = initialPos;
        velocity = initialVel;

        this->boundHeight = boundHeight;
        this->boundWidth = boundWidth;
        this->id = id;

        circle.setRadius(radius);
        circle.setOrigin(radius, radius);
        circle.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255, 255) );
        circle.setPosition(position);

    }
    void update() {
        velocity.y += 0.16; //gravity
        position.x += velocity.x;
        position.y += velocity.y;

        if (position.y + circle.getRadius() > boundHeight) {
            position.y = boundHeight - circle.getRadius();
            velocity.y *= -0.9;
            velocity.x *= 0.99;
        }

        if (position.x + circle.getRadius() > boundWidth) {
            position.x = boundWidth - circle.getRadius();
            velocity.x *= -0.9;
        }
        else if (position.x - circle.getRadius() < 0) {
            position.x = 0 + circle.getRadius();
            velocity.x *= -0.9;
        }

        if (velocity.x > 10) velocity.x = 10;
        if (velocity.y > 10) velocity.y = 10;

        circle.setPosition(position);
    }

    void setVelocity(sf::Vector2f newVelocity) {
        velocity.x = newVelocity.x;
        velocity.y = newVelocity.y;
    }

    sf::Vector2f getPosition() {
        return position;
    }

    float getRadius() {
        return circle.getRadius();
    }

    sf::Vector2f getVelocity() {
        return velocity;
    }

    int getId() {
        return id;
    }

    void tryColliding(Ball& otherBall) {
        //get distance between balls
        float dist = sqrt(
            pow(position.x - otherBall.getPosition().x, 2) 
            + pow(position.y - otherBall.getPosition().y, 2)
        );
        //collide the balls
        if (dist <= getRadius() + otherBall.getRadius()) {

            float R = 0.45;

            float xDiff = otherBall.getPosition().x - position.x;
            float yDiff = otherBall.getPosition().y - position.y;

            float velXDiff = otherBall.getVelocity().x - velocity.x;
            float velYDiff = otherBall.getVelocity().y - velocity.y;

            float velX_CM = (velocity.x + otherBall.getVelocity().x) / 2;
            float velY_CM = (velocity.y + otherBall.getVelocity().y) / 2;

            float fYDiff = 1.0E-12 * fabs(yDiff);
            if (fabs(xDiff) < fYDiff) {
                int sign = 1;
                if (xDiff < 0) { sign = -1;}
                xDiff = fYDiff * sign;
            }

            float angle = yDiff / xDiff;
            float dvx2 = -2 * (velXDiff + angle * velYDiff) / (1 + angle * angle);

            otherBall.setVelocity(sf::Vector2f(
                ((otherBall.getVelocity().x + dvx2) - velX_CM) * R + velX_CM,
                ((otherBall.getVelocity().y + angle * dvx2) - velY_CM) * R + velY_CM)
            );
            setVelocity(sf::Vector2f(
                ((velocity.x - dvx2) - velX_CM) * R + velX_CM,
                ((velocity.y - angle * dvx2) - velY_CM) * R + velY_CM)
            );

        }

    }

    void draw(sf::RenderWindow& window) {
        window.draw(circle);
    }

};

int main(int argc, char** argv)
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    sf::RenderWindow renderWindow(sf::VideoMode(WIDTH, HEIGHT), "ball");
    sf::Event event;
    sf::Clock clock;

    srand(time(NULL));

    float fps = 60.0f;
    float frameTimeMS = 1000.0f / fps;

    std::vector<Ball> balls;
    std::vector<Ball>::iterator it;
    std::vector<Ball>::iterator it2;
    int count;

    for (int i = 0; i < 20; i++) {
        Ball ball(20.0, sf::Vector2f(50 * (i + 1), 100 + rand() % 100 - 50), sf::Vector2f(rand() % 10 - 5, rand() % 10 - 5), HEIGHT, WIDTH, i);
        balls.push_back(ball);
    }

    while (renderWindow.isOpen()) {

        //poll events until no more events
        while (renderWindow.pollEvent(event)) {

            //if window closed event
            if (event.type == sf::Event::EventType::Closed) 
                renderWindow.close();

            //if keyboard event.type == sf::Event::EventType::KeyPressed
            //if event.key.code == sf::Keyboard::CODE

        }

        renderWindow.clear(); //before draw clear frame

        if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {

            //iterate through balls and update them and check for collisions

            it = balls.begin();
            it2 = balls.begin();
            count = 0;

            while (it != balls.end()) {

                for (int i = 0; i < count + 1; i++) it2++;

                while (it2 != balls.end()) {

                    it->tryColliding( (*it2) );

                    it2++;
                }

                it2 = balls.begin();
                it++;
                count++;
            }

            it = balls.begin();

            while (it != balls.end()) {
                it->update();
                it++;
            }

            clock.restart();
        }
        //iterate through balls and draw them
        std::for_each(balls.begin(), balls.end(), [&](Ball& b) {
            b.draw(renderWindow);
        });
        renderWindow.display(); //display new frame

    }

    //end of execution
}
//useful info
//sf::Color color(sf::Color::Red);
//srand(time(NULL));
//std::uniform_int_distribution<int> randomColorRange(0, 255); //light call
//std::random_device rd; //heavy call
//std::mt19937 randomNumbers(rd()); //medium call
// std::unordered_map<int, bool> keys; 