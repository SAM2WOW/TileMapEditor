#pragma once

#include <iostream>
#include <SFML\Graphics.hpp>

class Button {
public:
	Button() {}

	Button(sf::Vector2f position, std::string text, sf::Font &font, std::string iconPath = "") {
		button.setSize(sf::Vector2f(64.f, 64.f));
		
		button.setPosition(position);
		
		button.setFillColor(sf::Color(0x2d2f31ff));
		button.setOutlineThickness(0);

		if (iconPath != "") {
			icon.loadFromFile(iconPath);
			icon.setSmooth(true);
			iconSprite.setTexture(icon);
			iconSprite.setPosition(position + sf::Vector2f(30.f, 30.f));
		}

		buttonText.setString(text);
		
		buttonText.setFont(font);
		
		buttonText.setCharacterSize(15);
		buttonText.setPosition(position + sf::Vector2f(2.f, 0.f));
		buttonText.setFillColor(sf::Color(0xffffff99));

		pressed = false;
	}

	sf::Vector2f getPosition() {
		return button.getPosition();
	}

	void setPosition(sf::Vector2f position) {
		button.setPosition(position);
		buttonText.setPosition(position + sf::Vector2f(2.f, 0.f));
		iconSprite.setPosition(position + sf::Vector2f(30.f, 30.f));
	}

	void setText(std::string text) {
		buttonText.setString(text);
		buttonText.setCharacterSize(15);
		buttonText.setFillColor(sf::Color(0xffffff99));
	}

	void setFont(sf::Font &font) {
		buttonText.setFont(font);
	}

	void setIcon(std::string iconPath) {
		icon.loadFromFile(iconPath);
		icon.setSmooth(true);
		iconSprite.setTexture(icon);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(button);
		window.draw(buttonText);
		window.draw(iconSprite);
	}

	void changeColor(sf::Color color) {
		button.setFillColor(color);
	}
	
	bool isMouseOver(sf::RenderWindow& window) {
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		sf::FloatRect bounds = button.getGlobalBounds();

		if (bounds.contains(mousePos)) {
			button.setFillColor(sf::Color(0x4b5643ff));
			iconSprite.setScale(1.2f, 1.2f);
		} else {
			button.setFillColor(sf::Color(0x2d2f31ff));
			iconSprite.setScale(1.f, 1.f);
		}

		return bounds.contains(mousePos);
	}
	
	void press(bool state) {
		pressed = state;

		if (pressed) {
			//button.setSize(sf::Vector2f(100.f, 64.f));
			button.setFillColor(sf::Color(0x4b5643ff));
			button.setOutlineThickness(2);
			button.setOutlineColor(sf::Color(0xdcfbaaff));
			//iconSprite.setPosition(button.getPosition() + sf::Vector2f(60.f, 30.f));
		} else {
			//button.setSize(sf::Vector2f(64.f, 64.f));
			button.setFillColor(sf::Color(0x2d2f31ff));
			button.setOutlineThickness(0);
			//iconSprite.setPosition(button.getPosition() + sf::Vector2f(30.f, 30.f));
		}
	}

	bool isPressed() {
		return pressed;
	}

private:
	sf::RectangleShape button;
	sf::Text buttonText;
	sf::Texture icon;
	sf::Sprite iconSprite;
	bool pressed;
};