
#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>

class MySquare {

private:

    sf::VertexArray square;
    std::vector<std::vector<float>> absCoords;
    int vCount;

public:

    MySquare() {
        vCount = 0;
    }

    MySquare(std::vector<std::vector<float>> newCoords) {
        vCount = 0;
        setVertices(newCoords);
    }

    void addVertex(std::vector<float>& coord) {
        std::cout << "adding vertex " << coord[0] << ", " << coord[1] << ", " << coord[2] << std::endl;
        square.append(sf::Vertex());
        square[vCount].position = sf::Vector2f(coord[0], coord[1]);
        square[vCount].color = sf::Color::Red;
        absCoords[vCount] = { coord[0], coord[1], coord[2]};
        vCount++;
        std::cout << "vertex added" << std::endl;

    }

    void setVertices(std::vector<std::vector<float>> newCoords) {
        std::cout << "setting vertices" << std::endl;
        vCount = 0;
        for (int i = 0; i < newCoords.size(); i++) {
            addVertex(newCoords[i]);
        }
    }

    void draw(sf::RenderWindow&) {

    }

    void updatePerspective(float xOffsetAngle, float yOffsetAngle) {



    }

    
};

int main()
{
    const int c_width = 600;
    const int c_height = 480;

    const float desiredFPS = 10.0;
    float frameTimeMS = 1000.0 / desiredFPS;

    sf::RenderWindow window(sf::VideoMode(c_width, c_height), "Title");

    sf::Event event;

    sf::Clock clock;

    std::vector<MySquare> squares;

    std::vector<std::vector<float>> coords =
    {
        {100.0, 100.0, 0.0},
        {200.0, 100.0, 0.0},
        {200.0, 200.0, 0.0},
        {100.0, 200.0, 0.0}
    };

    MySquare square(coords);

    squares.push_back(square);

    std::vector<MySquare>::iterator it;
    

    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {

            window.clear();

            it = squares.begin();
            while (it != squares.end()) {
                it->draw(window);
                it++;
            }
            
            window.display();

            clock.restart();
        }
    }
}

