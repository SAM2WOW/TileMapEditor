#pragma once

#include <iostream>
#include <SFML\Graphics.hpp>

class Tile {
public:
	Tile() {}

	Tile(sf::Vector2f position, std::string texturePath = "") {
		if (texturePath != "") {
			texture.loadFromFile(texturePath);
			// texture.setSmooth(true);
			sprite.setTexture(texture);
			sprite.setPosition(position);

			// set the scale to 64
			sf::Vector2u originalSize = texture.getSize();
			float newScale = 64.f / originalSize.x;
			sprite.setScale(sf::Vector2f(newScale, newScale));
		}
	}

	void draw(sf::RenderWindow& window) {
		window.draw(sprite);
	}

	void setPosition(sf::Vector2f position) {
		sprite.setPosition(position);
	}

	void setOpacity(int opacity) {
		sprite.setColor(sf::Color(255, 255, 255, opacity));
	}

private:
	sf::Texture texture;
	sf::Sprite sprite;
};