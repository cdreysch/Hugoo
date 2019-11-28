#ifndef myTileMap_h
#define myTileMap_h myTileMap_h

#include <SFML/Graphics.hpp>
#include "myTileMap.h"

class myTileMap : public sf::Drawable, public sf::Transformable{
    public:
	bool load(const std::string& tileset, sf::Vector2u tileSize, std::vector<int>* tiles, unsigned int width, unsigned int height);
	int changeTile(sf::Vector2f pos, unsigned int tileNumber);
    private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
	    states.transform *= getTransform(); // apply the transform
	    states.texture = &m_tileset; // apply the tileset texture
	    target.draw(m_vertices, states); // draw the vertex array
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
	sf::Vector2u m_tilesize;
	unsigned int m_width; 
	unsigned int m_height;
};
#endif
