
#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>
#include <execution>



class Agent {

public:

    sf::Color color;

    sf::Vector2f position;

    sf::Vector2f velocity;

    //global agent parameters
    static float attractionMod; //avg intensity risen to the power of this
    static float similarColorAttractionMod; //similar pixel score to the power of this
    static float turnWeightChangeRate; //how much turn weight is changed in some direction
    static float maxTurnRate; //degrees per update
    const float rayLength = 10.0f; //distance agent can detect pixels
    
    Agent() {

    }

    Agent(sf::Vector2f pos, int xBound, int yBound, int id, sf::Color color) {
        position = pos;
        this->xBound = xBound;
        this->yBound = yBound;
        this->id = id;
        this->color = color;

        redGreenRatio = color.r / (color.g == 0 ? 1 : color.g);

        //make a vector in a random direction
        velocity = sf::Vector2f(
           (float) (rand() % 2000 - 1000) / 1000.0f,
            (float) (rand() % 2000 - 1000) / 1000.0f
        );

        //normalize vector to length 1
        float hyp = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        velocity.x /= hyp;
        velocity.y /= hyp;
        turnWeight = 0;
    }

    void updateDirection(sf::Image& image) {
        int width = image.getSize().x;
        int height = image.getSize().y;
        float direction = atan2f(velocity.y, velocity.x); //radians

        //check vectors directly in front, 30 deg left, and 30 deg right
        //val = (0.0 - 1.0) total rgb val, -1 = boundary
        pixelGroups.clear();
        pixelGroups.push_back(rayCast(position, direction - (std::_Pi / 6), rayLength)); //left direction 30 deg
        pixelGroups.push_back(rayCast(position, direction, rayLength)); //forward direction
        pixelGroups.push_back(rayCast(position, direction + (std::_Pi / 6), rayLength)); //right direction 30 deg
        
        //influences: 0 = left, 1 = mid, 2 = right
        bool bound[3];      
        long val[3];      
        float intensity[3]; //averge of the sums of rgb values for a set of pixels
        int similarPixels[3];

        for (long i = 0; i < 3; i++) {
            intensity[i] = 0;
            val[i] = 1;
            bound[i] = false;
            similarPixels[i] = 0;
        }

        for (int i = 0; i < 3; i++) {
            std::vector<sf::Vector2i> group = pixelGroups[i];
            for (sf::Vector2i pixelCoord : group) {

                if (pixelCoord.x < 0 || pixelCoord.x >= width || pixelCoord.y < 0 || pixelCoord.y >= height) { //pixel out of bounds
                    bound[i] = true;
                    val[i] = 1;
                    break;
                }

                sf::Color pixel = image.getPixel(pixelCoord.x, pixelCoord.y);

                if (abs(color.r - pixel.r) < 50 && abs(color.g - pixel.g) < 50 && abs(color.b - pixel.b) < 50) {
                    similarPixels[i] += 200;
                }

                intensity[i] += (pixel.r + pixel.g + pixel.b);
            }

            intensity[i] /= group.size(); //avg pixel intensity will be [0 - 765]
        }

        //add weight to all = their avg intensity ^ attraction modifier (higher values weigh more exponentially)
        for (int i = 0; i < 3; i++) {
            if (!bound[i]) {
                val[i] += pow(intensity[i], attractionMod);
                val[i] += pow(similarPixels[i], similarColorAttractionMod);
            }
        }

        int result = (rand()) % (val[0] + val[1] + val[2]);

        //alter turn weight depending on winning weight
        if (result < val[1]) { //left wins steer turnWeight towards -1
            turnWeight = std::max(-1.0f, turnWeight - turnWeightChangeRate);
        }
        else if (result < (val[0] + val[1])) { //mid wins steer turnWeight towards 0
            if (turnWeight < 0)
                turnWeight = std::min(0.0f, turnWeight + turnWeightChangeRate);
            else
                turnWeight = std::max(0.0f, turnWeight - turnWeightChangeRate);
        }
        else { //right wins steer turnWeight towards 1
            turnWeight = std::min(1.0f, turnWeight + turnWeightChangeRate);
        }

        //update velocity, direction changed by (pi/180) * turnRate * turnWeight
        float newDirection = direction + ((std::_Pi / 180) * maxTurnRate * turnWeight);
        velocity.x = cosf(newDirection);
        velocity.y = sinf(newDirection);
    }

    void updatePosition() {
        position.x = position.x + velocity.x * speed;
        position.y = position.y + velocity.y * speed;

        if (rand() % 1000 < 30) {
            position.x += rand() % 200 - 100;
            position.y += rand() % 200 - 100;
        }
        
        if (position.x < 0) {
            position.x = 0;
            velocity.x *= -1;
        }
        else if (position.x >= xBound) {
            position.x = xBound - 1;
            velocity.x *= -1;
        }

        if (position.y < 0) {
            position.y = 0;
            velocity.y *= -1;
        }
        else if (position.y >= yBound) {
            position.y = yBound - 1;
            velocity.y *= -1;
        }

        
    }

    void drawRaysToImage(sf::Image& image) {
        for (std::vector<sf::Vector2i> group : pixelGroups) {
            for (sf::Vector2i pixelCoord : group) {
                image.setPixel(
                    (pixelCoord.x < 0 //ensure x is within bounds
                        ? 0
                        : (pixelCoord.x >= xBound
                            ? xBound - 1
                            : pixelCoord.x
                        )
                    ),
                    (pixelCoord.y < 0 //ensure y is within bounds
                        ? 0
                        : (pixelCoord.y >= yBound
                            ? yBound - 1
                            : pixelCoord.y
                        )
                    ),
                    sf::Color::Green
                );
            }
        }
    }

    int getId() {
        return id;
    }

private:
    std::vector<std::vector<sf::Vector2i>> pixelGroups;

    int xBound;
    int yBound;

    int id;

    float speed = 1;
    float redGreenRatio;

    float turnWeight = 0; //(-1.0 - 1.0) a predisposition to go left or right

    //returns integer of next grid position in the positive direction from val
    int roundToGridPos(float val, int gridSize) {
        return (int)val + gridSize - ((int)val % gridSize);
    };

    //returns integer of next grid pos in negative direction from val
    int roundToGridNeg(float val, int gridSize) {
        return (int)val - ((int)val % gridSize);
    };

    std::vector<sf::Vector2i> rayCast(sf::Vector2f startPos, float angle, float distance) {
        int gridSize = 1;

        sf::Vector2f endPos(
            startPos.x + distance * cosf(angle),
            startPos.y + distance * sinf(angle)
        );
        //do the raycast:
        //determine axes directions
        sf::Vector2i direction(0, 0);
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

        std::vector<sf::Vector2i> intersections;

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
        intersections.push_back(sf::Vector2i(
            roundToGridNeg(startPos.x, gridSize),
            roundToGridNeg(startPos.y, gridSize)
        ));

        //go through x and y values and add the corresponding tile to intersections, offset depending on direction
        if (direction.x != 0) {
            int end = abs(xEnd - xCurrent);
            int offset = (direction.x < 0 ? gridSize : 0);
            for (int i = 0; i <= end; i += gridSize) {
                float yIntercept = (slope * xCurrent) + yOffset;
                intersections.push_back(sf::Vector2i(
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
                intersections.push_back(sf::Vector2i(
                    roundToGridNeg(xIntercept, gridSize),
                    yCurrent - offset
                ));
                yCurrent += gridSize * direction.y;
            }
        }

        return intersections;
    }
};

float Agent::attractionMod = 2.0f;
float Agent::turnWeightChangeRate = 0.5f;
float Agent::maxTurnRate = 15.0f;
float Agent::similarColorAttractionMod = 2.0f;

//decrease intensity of all pixels in image
void dimImage(sf::Image& image, std::vector<int> xvals, int dimAmount) {

    sf::Vector2u size = image.getSize();

    std::for_each(std::execution::par_unseq, xvals.begin(), xvals.end(), [&](int x) {
        for (int y = 0; y < size.y; y++) {
            sf::Color currentPixel = image.getPixel(x, y);
            currentPixel.r = std::max(0, currentPixel.r - dimAmount);
            currentPixel.g = std::max(0, currentPixel.g - dimAmount);
            currentPixel.b = std::max(0, currentPixel.b - dimAmount);
            image.setPixel(x, y, currentPixel);
        }
    });
}

int main()
{
    const int c_width = 600;
    const int c_height = 480;
    const float desiredFPS = 100.0;
    const float scale = 2;

    const int agentCount = 500;

    int trailDimAmount = 5;

    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(c_width * scale, c_height * scale), "Ant");

    sf::Texture texture;
    sf::Texture rayTexture;

    if (!texture.create(c_width, c_height)) {
        std::cout << "texture error" << std::endl;
    }

    if (!rayTexture.create(c_width, c_height)) {
        std::cout << "texture error" << std::endl;
    }

    sf::Sprite sprite;
    sprite.setScale(sf::Vector2f(scale, scale));

    std::vector<int> xvals;
    for (int i = 0; i < c_width; i++)
        xvals.push_back(i);

    sf::Image image;
    sf::Image imagePrev;

    sf::Event event;

    sf::Clock clock;

    sf::Font font;
    if (!font.loadFromFile("Roboto-Light.ttf")) {
        std::cout << "font error" << std::endl;
    }

    int fps = 10;

    sf::Text fpsCounter(std::to_string(fps), font, 30);
    fpsCounter.setFillColor(sf::Color::Red);

    sf::Text selectedPropertyText("No Property", font, 30);
    selectedPropertyText.setFillColor(sf::Color::Red);
    selectedPropertyText.setPosition(sf::Vector2f(0, 50));

    bool drawDebug = false;
    
    float frameTimeMS = 1000.0 / desiredFPS;

    std::vector<Agent> agents;

    for (int i = 0; i < agentCount; i++) {
        agents.push_back(Agent(sf::Vector2f(rand() % c_width, rand() % c_height), c_width, c_height, i, sf::Color(rand() % 256, rand() % 256, 255) ) );
    }

    int selectedProperty = 0;

    while (window.isOpen()) {

        while (window.pollEvent(event)) {

            if(event.type == sf::Event::Closed)
                window.close();

            //keys to adjust parameters
            //0:none, 1:attractionMod, 2:turnRateOfChange 3:maxTurn
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Num0) {
                    selectedProperty = 0;
                    selectedPropertyText.setString("No Property");
                }
                if (event.key.code == sf::Keyboard::Num1) {
                    selectedProperty = 1;
                    selectedPropertyText.setString("AtrMod: " + std::to_string(Agent::attractionMod));
                }
                if (event.key.code == sf::Keyboard::Num2) {
                    selectedProperty = 2;
                    selectedPropertyText.setString("TurnChgRt: " + std::to_string(Agent::turnWeightChangeRate));
                }
                if (event.key.code == sf::Keyboard::Num3) {
                    selectedProperty = 3;
                    selectedPropertyText.setString("MaxTurn: " + std::to_string(Agent::maxTurnRate));
                }
                if (event.key.code == sf::Keyboard::Num4) {
                    selectedProperty = 4;
                    selectedPropertyText.setString("TrailDim: " + std::to_string(trailDimAmount));
                }
                if (event.key.code == sf::Keyboard::Num5) {
                    selectedProperty = 5;
                    selectedPropertyText.setString("SelfAtrMod: " + std::to_string(Agent::similarColorAttractionMod));
                }
                if (event.key.code == sf::Keyboard::Up) {
                    if (selectedProperty == 1) {
                        Agent::attractionMod += 0.1f;
                        selectedPropertyText.setString("AtrMod: " + std::to_string(Agent::attractionMod));
                    }
                    else if (selectedProperty == 2) {
                        Agent::turnWeightChangeRate += 0.05f;
                        selectedPropertyText.setString("TurnChgRt: " + std::to_string(Agent::turnWeightChangeRate));
                    }
                    else if (selectedProperty == 3) {
                        Agent::maxTurnRate += 0.5f;
                        selectedPropertyText.setString("MaxTurn: " + std::to_string(Agent::maxTurnRate));
                    }
                    else if (selectedProperty == 4) {
                        trailDimAmount += 1;
                        selectedPropertyText.setString("TrailDim: " + std::to_string(trailDimAmount));
                    }
                    else if (selectedProperty == 5) {
                        Agent::similarColorAttractionMod += 0.1f;
                        selectedPropertyText.setString("SelfAtrMod: " + std::to_string(Agent::similarColorAttractionMod));
                    }
                }
                if (event.key.code == sf::Keyboard::Down) {
                    if (selectedProperty == 1) {
                        Agent::attractionMod -= 0.1f;
                        selectedPropertyText.setString("AtrMod: " + std::to_string(Agent::attractionMod));
                    }
                    else if (selectedProperty == 2) {
                        Agent::turnWeightChangeRate -= 0.05f;
                        selectedPropertyText.setString("TurnChgRt: " + std::to_string(Agent::turnWeightChangeRate));
                    }
                    else if (selectedProperty == 3) {
                        Agent::maxTurnRate -= 0.5f;
                        selectedPropertyText.setString("MaxTurn: " + std::to_string(Agent::maxTurnRate));
                    }
                    else if (selectedProperty == 4) {
                        trailDimAmount -= 1;
                        selectedPropertyText.setString("TrailDim: " + std::to_string(trailDimAmount));
                    }
                    else if (selectedProperty == 5) {
                        Agent::similarColorAttractionMod -= 0.1f;
                        selectedPropertyText.setString("SelfAtrMod: " + std::to_string(Agent::similarColorAttractionMod));
                    }
                }
                if (event.key.code == sf::Keyboard::D)
                    drawDebug = !drawDebug;
            }

        }

        if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {

            clock.restart();

            image = texture.copyToImage();
            imagePrev = image;

            //update agents
            std::for_each(std::execution::par_unseq, agents.begin(), agents.end(), [&](auto&& agent) {
                image.setPixel(agent.position.x, agent.position.y, agent.color);

                agent.updateDirection(imagePrev);

                agent.updatePosition();
            });

            window.clear();

            dimImage(image, xvals, trailDimAmount);

            texture.loadFromImage(image);
            sprite.setTexture(texture);
            
            window.draw(sprite);

            if (drawDebug) {
                window.draw(fpsCounter);
                window.draw(selectedPropertyText);
            }

            window.display();

            fps = clock.getElapsedTime().asMilliseconds();
            fpsCounter.setString(std::to_string(fps) + "ms");
        }
        
    }
}

