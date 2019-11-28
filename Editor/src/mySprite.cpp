#include "mySprite.h"
#include "mySpriteManager.h"

mySprite::mySprite(int x, int y, mySpritesheet* ptr){
  quad.resize(4);	
  position = sf::Vector2i(x,y);	
  setSpritesheetPtr(ptr); 			
  setColor(sf::Color(255,255,255,255));	
  setCurrentTextureIndex(0);		
  setCurrentRectIndex(0);	
  layer = 0;
  spriteManagerPtr = NULL;
}

sf::Vector2i mySprite::getDrawingPosition(){		
  int xd = position.x+spriteSheetPtr->getOffsetPtrAt(currentRectIndex)->x;
  int yd = position.y+spriteSheetPtr->getOffsetPtrAt(currentRectIndex)->y;	

  return sf::Vector2i(xd,yd);
}

void mySprite::setPosition(int x, int y){
  position = sf::Vector2i(x,y);	
  setQuadPosition();
}

void mySprite::setColor(sf::Color newcolor){
  color = newcolor;
  setQuadColor();
}

void mySprite::setQuadTexCoords(){	
  int tx = spriteSheetPtr->getTilePtrAt(currentRectIndex)->left;
  int ty = spriteSheetPtr->getTilePtrAt(currentRectIndex)->top;
  int width = spriteSheetPtr->getTilePtrAt(currentRectIndex)->width;
  int height = spriteSheetPtr->getTilePtrAt(currentRectIndex)->height;

  quad[0].texCoords = sf::Vector2f(tx, ty);
  quad[1].texCoords =  sf::Vector2f(tx+width, ty);
  quad[2].texCoords =  sf::Vector2f(tx+width, ty+height);
  quad[3].texCoords = sf::Vector2f(tx,  ty+height);
}

void mySprite::setQuadPosition(){	
  int x = getDrawingPosition().x;
  int y = getDrawingPosition().y;
  int width = spriteSheetPtr->getTilePtrAt(currentRectIndex)->width;
  int height =spriteSheetPtr->getTilePtrAt(currentRectIndex)->height;

  if (width<0)
    width *= -1;

  if (height<0)
    height *= -1;

  quad[0].position = sf::Vector2f(x, y);
  quad[1].position = sf::Vector2f(x+width, y);
  quad[2].position = sf::Vector2f(x+width, y+height);
  quad[3].position = sf::Vector2f(x,  y+height);
}

void mySprite::setQuadColor(){
  quad[0].color = color;
  quad[1].color = color;
  quad[2].color = color;
  quad[3].color = color;
}

void mySprite::setSpriteManagerPtr(mySpriteManager* ptr) {
  spriteManagerPtr = ptr;
}

void mySprite::setLayer(int i) {
  layer = i;
  if(spriteManagerPtr!=NULL) {
    spriteManagerPtr->sort();
  }
}
