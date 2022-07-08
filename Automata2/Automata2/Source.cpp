#include "SFML\Graphics.hpp"
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <math.h>
#include <execution>

class Blob {
public:
	sf::Color color;
	int id;
	Blob(int id) {
		color = sf::Color(rand() % 256, rand() % 256, rand() % 256, 255);
		this->id = id;
	}

private:

};

void syncToData(sf::Image image, std::vector<std::vector<int>>& data, std::vector<int> xvals, std::vector<sf::Color> colors) {
	std::for_each(std::execution::par, xvals.begin(), xvals.end(), [&](int x) {
		for (int y = 0; y < image.getSize().y; y++) {
			image.setPixel(x, y, colors[data[x][y]]);
		}
	});
}

int main() {
	
	const int c_width = 600;
	const int c_height = 480;
	const int gridSize = 10;
	const float scale = 2.0f;
	const float fps = 10;
	const float frameTimeMS = 1000 / fps;

	const int attackChance = 20;

	const int teamCount = 3;

	std::vector<sf::Color> colors = {sf::Color::Black, sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Cyan, sf::Color::Magenta };

	std::vector<std::vector<int>> data;
	
	std::vector<int> xvals;
	for (int x = 0; x < c_width; x++) {
		std::vector<int> ydata;
		for (int y = 0; y < c_height; y++)
			ydata.push_back(0);
		data.push_back(ydata);
		xvals.push_back(x);
	}

	std::vector<std::vector<int>> dataBuffer = data;

	sf::RenderWindow window(sf::VideoMode(c_width * scale, c_height * scale), "thing");

	sf::Event event;

	sf::Clock clock;

	sf::Texture canvas;

	if (!canvas.create(c_width, c_height)) {
		std::cout << "texture load error" << std::endl;
	}

	srand(time(NULL));

	sf::Image canvasImage = canvas.copyToImage();
	for(int i = 0; i < 1; i++)
	for (int i = 1; i <= teamCount; i++) {
		int x = rand() % c_width;
		int y = rand() % c_height;
		data[x][y] = i;
		canvasImage.setPixel(x, y, colors[i]);

	}
	canvas.loadFromImage(canvasImage);

	sf::Sprite canvasSprite;
	canvasSprite.setScale(sf::Vector2f(scale, scale));

	while (window.isOpen()) {

		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		if (clock.getElapsedTime().asMilliseconds() >= frameTimeMS) {

			canvasImage = canvas.copyToImage();

			dataBuffer = data;
			std::for_each(std::execution::par, xvals.begin(), xvals.end(), [&](int x) {
				for (int y = 0; y < c_height; y++) {
					if (data[x][y] != 0) {
						int enemy = (data[x][y] == 1 ? teamCount : data[x][y] - 1);
						for (int i = x - 1; i <= x + 1; i++) {
							if (i < 0 || i >= c_width)
								continue;
							for (int j = y - 1; j <= y + 1; j++) {
								if (j < 0 || j >= c_height || (j == y && i == x) || rand() % 100 > attackChance)
									continue;
								if (data[i][j] == enemy || data[i][j] == 0) {
									dataBuffer[i][j] = data[x][y];
									canvasImage.setPixel(x, y, colors[data[x][y]]);
								}
							}
						}
					}
				}
			});
			data = dataBuffer;

			window.clear();

			syncToData(canvasImage, data, xvals, colors);

			canvas.loadFromImage(canvasImage);
			canvasSprite.setTexture(canvas);

			window.draw(canvasSprite);
			window.display();
			clock.restart();
		}

	}

}