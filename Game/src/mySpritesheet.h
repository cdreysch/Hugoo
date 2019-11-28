#ifndef mySpritesheet_h
#define mySpritesheet_h mySpritesheet_h
#include <SFML/Graphics.hpp>
#include <vector>
#include "mySequence.h"

class mySpritesheet {
  public:
    mySpritesheet()						{}
    mySpritesheet(std::string filename)				{addTexture(filename);}

    bool addTexture(std::string filename);
    void defineTile(int x, int y, int width, int height, int factor=1, int xoff=0, int yoff=0);
    void addSequencePtr(mySequence* ptr);

    sf::Texture* getTexturePtrAt(unsigned int index)		{return &textures.at(index);}
    sf::IntRect* getTilePtrAt(unsigned int index)		{return &tiles.at(index);}
    sf::Vector2i* getOffsetPtrAt(unsigned int index)		{return &offsets.at(index);}
    mySequence* getSequencePtrAt(unsigned int index)		{return sequencePtr.at(index);}
    unsigned int definedTiles()					{return tiles.size();}
    unsigned int definedSequences()				{return sequencePtr.size();}

  private:
    std::vector<sf::Texture> textures;
    std::vector<sf::IntRect> tiles;
    std::vector<sf::Vector2i> offsets;
    std::vector<mySequence*> sequencePtr;
};
#endif
