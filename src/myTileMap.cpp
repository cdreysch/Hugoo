#include "myTileMap.h"

bool myTileMap::load(const std::string& tileset, sf::Vector2u tileSize, std::vector<int>* tiles, unsigned int width, unsigned int height)    {
    // load the tileset image
    sf::Image tilesetImage;
    if (!tilesetImage.loadFromFile(tileset))
	return false;		
    tilesetImage.createMaskFromColor(tilesetImage.getPixel(0,0));

    // load the tileset texture
    if (!m_tileset.loadFromImage(tilesetImage))
	return false;

    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(width * height * 4);

    // populate the vertex array, with one quad per tile
    for (unsigned int i = 0; i < width; ++i)
	for (unsigned int j = 0; j < height; ++j){

	    // get the current tile number
	    int tileNumber = tiles->at(i + j * width);

	    // find its position in the tileset texture
	    int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
	    int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

	    // get a pointer to the current tile's quad
	    sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

	    // define its 4 corners
	    quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
	    quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
	    quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
	    quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

	    // define its 4 texture coordinates
	    quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
	    quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
	    quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
	    quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
	}

    m_tilesize = tileSize;
    m_width = width;
    m_height = height;
    return true;
}

int myTileMap::changeTile(sf::Vector2f pos, unsigned int tileNumber)    {
    int x = (int)pos.x/m_tilesize.x;
    int y = ((int)pos.y/m_tilesize.y)+2;

    // find its position in the tileset texture
    int tu = tileNumber % (m_tileset.getSize().x / m_tilesize.x);
    int tv = tileNumber / (m_tileset.getSize().x / m_tilesize.x);

    // get a pointer to the tile's quad
    sf::Vertex* quad = &m_vertices[(x + y * m_width) * 4];

    // define its 4 texture coordinates
    quad[0].texCoords = sf::Vector2f(tu * m_tilesize.x, tv * m_tilesize.y);
    quad[1].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, tv * m_tilesize.y);
    quad[2].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, (tv + 1) * m_tilesize.y);
    quad[3].texCoords = sf::Vector2f(tu * m_tilesize.x, (tv + 1) * m_tilesize.y);
    return x;
}
