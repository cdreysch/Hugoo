#pragma once

#include "SrcImage.h"
#include <SFML/Graphics.hpp>


class Tool {
public:
	sf::Color pen_color;
	sf::RenderWindow *window;
	Tool(sf::RenderWindow *target) {
		window = target;
		pen_color = sf::Color(0, 0, 0);
	}
	sf::Vector2f getPosition() {
		return window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	}
	sf::Color getColor() { return pen_color; }
	void setColor(sf::Color color) { pen_color = color; }
};


class Canvas : public sf::Sprite
{
private:
	int layer;
public:	
	~Canvas();
	std::string name;	
	SrcImage *p_srcImage;
	sf::Texture srcTexture;

	Canvas(std::string id_string = "noname_canvas");

	void setPixel(Tool *tool);
	void setPixel(int x, int y, sf::Color color);
	sf::Color getPixel(int x, int y);
	void loadTexture(SrcImage *srcImage);
	bool contains(float x, float y);
	inline int getLayer() { return layer; }
	inline void setLayer(int value) { layer = value; }
};

