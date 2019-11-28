#include "mySpritesheet.h"
#include <iostream>

bool mySpritesheet::addTexture(std::string filename){
  sf::Image srcImage;			
  if (!srcImage.loadFromFile(filename)){
    std::cout << "Datei nicht gefunden: " << filename<< std::endl;
    return 0;
  }	
  srcImage.createMaskFromColor(sf::Color(0,0,0));	
  sf::Texture texture;
  if (!texture.loadFromImage(srcImage)){
    return 0;
  }
  textures.push_back(texture);
  return 1;
}

void mySpritesheet::addSequencePtr(mySequence* ptr){
  sequencePtr.push_back(ptr);
}

void mySpritesheet::defineTile(int x, int y, int width, int height, int factor, int xoff, int yoff){
  tiles.emplace_back(sf::IntRect(x*factor,y*factor,width*factor,height*factor));
  offsets.emplace_back(sf::Vector2f(xoff*factor,yoff*factor));	
}
