#include "mySprite.h"
#include "mySpriteManager.h"

bool compare_sprite_layers(mySprite* a,mySprite* b){
  return (a->getLayer()<b->getLayer());
}

void mySpriteManager::draw(sf::RenderWindow *target){
  for(unsigned int i = 0; i<spritePtr.size();i++){
    target->draw(*spritePtr.at(i));
  }
}

void mySpriteManager::insert(mySprite* ptr){
  ptr->setSpriteManagerPtr(this);
  spritePtr.push_back(ptr);
  sort();
}

void mySpriteManager::sort(){
  std::stable_sort(spritePtr.begin(),spritePtr.end(),compare_sprite_layers);
}
