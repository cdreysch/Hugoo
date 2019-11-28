#include "myItem.h"
#include "myHuman.h"

myItem::myItem(int x, int y, mySpritesheet* ptr){
  sprite = myAnimatedSprite(x,y,ptr);
  color = sf::Color(55,155,55,255);
  sprite.setColor(color);
  ssp_nouser = ptr;
  user = NULL;
}

void myItem::update(){
  sprite.update();
  position = sprite.getPosition();
}

void myItem::draw(sf::RenderWindow *target){
  target->draw(sprite);
}

void myItem::wearedBy(myHuman* whom){
  user = whom;
    setPosition(user->getPosition());
    setCurrentTextureIndex(user->getCurrentTextureIndex());
    setCurrentSequenceIndex(user->getCurrentStatus());
    sprite.setSpritesheetPtr(ssp_wear);
}

void myItem::unwearedBy(myHuman* whom){
  user = NULL;
    nouser();
    setCurrentTextureIndex(0);
    setCurrentSequenceIndex(0);
}

void myItem::grabbedBy(myHuman* whom){
  user = whom;
    setPosition(user->getPosition());
    setCurrentTextureIndex(user->getCurrentTextureIndex());
    setCurrentSequenceIndex(user->getCurrentStatus());
    sprite.setSpritesheetPtr(ssp_carry);
}
void myItem::droppedBy(myHuman* whom){
  user = NULL;
    nouser();
    setCurrentTextureIndex(0);
    setCurrentSequenceIndex(0);
}

bool myItem::isUsed(){
  if(user==NULL) return false;
  else return true;
}
