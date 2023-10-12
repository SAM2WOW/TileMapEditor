#pragma once

#include <iostream>
#include <SFML\Graphics.hpp>

class Tile {
public:
	Tile() {}

	Tile(sf::Vector2f position, sf::Texture& texture) {
		sprite.setTexture(texture);
		sprite.setPosition(position);

		// set the scale to 64
		sf::Vector2u originalSize = texture.getSize();
		float newScale = 64.f / originalSize.x;
		sprite.setScale(sf::Vector2f(newScale, newScale));
	}

	void draw(sf::RenderWindow& window) {
		window.draw(sprite);
	}

	void setPosition(sf::Vector2f position) {
		sprite.setPosition(position);
	}

	void setTexture(sf::Texture& texture) {
		sprite.setTexture(texture);

		// set the scale to 64
		sf::Vector2u originalSize = texture.getSize();
		float newScale = 64.f / originalSize.x;
		sprite.setScale(sf::Vector2f(newScale, newScale));
	}

	void setOpacity(int opacity) {
		sprite.setColor(sf::Color(255, 255, 255, opacity));
	}

private:
	sf::Sprite sprite;
};