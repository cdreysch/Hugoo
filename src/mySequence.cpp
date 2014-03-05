#include "mySequence.h"

void mySequence::addEntry(unsigned int triggerFrame, int x, int y, int width, int height, int dispoX, int dispoY, int factor, int moveX, int moveY) {
  triggerFrames.emplace_back(triggerFrame);
  textureRects.emplace_back(sf::IntRect(x*factor,y*factor,width*factor,height*factor));
  textureDispositions.emplace_back(sf::Vector2f(dispoX*factor,dispoY*factor));
  movesf.emplace_back(sf::Vector2i(moveX,moveY));
}

void mySequence::addEntry(unsigned int triggerFrame, unsigned int index,  int moveX, int moveY) {
  triggerFrames.emplace_back(triggerFrame);  
  moves.emplace_back(sf::Vector2i(moveX,moveY));
  rectIndices.emplace_back(index);
}
