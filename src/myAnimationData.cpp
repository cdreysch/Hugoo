#include "myAnimationData.h"

myAnimationData::myAnimationData(std::string filename){
  addTexture(filename);
}	

void myAnimationData::addSequence(mySequence &seq) {
  availableSequences.push_back(seq);
}

void myAnimationData::addTexture(std::string filename) {
  sf::Image srcImage;			
  if (!srcImage.loadFromFile(filename)){
    // error...
  }	
  srcImage.createMaskFromColor(sf::Color(0,0,0));	
  sf::Texture texture;
  if (!texture.loadFromImage(srcImage)){
    // error...
  }
  textures.push_back(texture);
}
