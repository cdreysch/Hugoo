#pragma once


#include <map>
#include <SFML/Graphics.hpp>

class Canvas;

class SrcImage : public sf::Image
{
public:
	
	std::map<std::string, Canvas*> clientCanvases;
	std::map<std::string, Canvas*>::iterator clientCanvases_it;
	void addClientCanvas(std::string name, Canvas* p_canvas);
	void removeClientCanvas(std::string name);
	void updateAllClients();
};

