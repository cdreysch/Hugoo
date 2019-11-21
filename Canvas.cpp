#include "Canvas.h"
#include <iostream>


Canvas::Canvas(std::string id_string) {
	name = id_string;
	layer = 0;
	p_srcImage = NULL;
	srcTexture = sf::Texture();
	srcTexture.create(10, 10);
}


void Canvas::setPixel(Tool *tool) {
	setPixel(tool->getPosition().x, tool->getPosition().y, tool->getColor());
}
void Canvas::setPixel(int x, int y, sf::Color color) {
	//sf::Color old_color = getPixel(x, y);
	//editing_history.emplace_back(EditingStep(&(p_srcImage->setPixel), &(p_srcImage->setPixel)));
	p_srcImage->setPixel(getTextureRect().left + x - getPosition().x, getTextureRect().top + y - getPosition().y, color);
	srcTexture.update(*p_srcImage);
}
sf::Color Canvas::getPixel(int x, int y) {
	return p_srcImage->getPixel(getTextureRect().left + x - getPosition().x, getTextureRect().top + y - getPosition().y);
	std::cout << " x: " << x << " y: " << y << std::endl;
}
void Canvas::loadTexture(SrcImage *srcImage) {
	p_srcImage = srcImage;
	p_srcImage->addClientCanvas(name, this);
	srcTexture.loadFromImage(*p_srcImage);
	setTexture(srcTexture);
}
bool Canvas::contains(float x, float y) {
	if (x >= getPosition().x
		&& x <= getPosition().x + getTextureRect().width
		&& y >= getPosition().y
		&& y <= getPosition().y + getTextureRect().height) {
		return true;
	}
	else {
		return false;
	}
}


Canvas::~Canvas()
{
}
