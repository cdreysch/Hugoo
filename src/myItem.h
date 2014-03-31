#ifndef myItem_h
#define myItem_h myItem_h

#include <cstddef>  //NULL
#include <SFML/Graphics.hpp>
#include "mySpritesheet.h"
#include "myAnimatedSprite.h"

class myHuman;

class myItem {
  public:
    myItem() {}
    myItem(int x, int y, mySpritesheet* ptr);

    sf::Vector2i getPosition() {return sprite.getPosition();}
    mySprite* getSpritePtr() {return &sprite;}
    
    void setCurrentTextureIndex(unsigned int index) {sprite.setCurrentTextureIndex(index);}
    void setPosition(sf::Vector2i pos) {position = pos; sprite.setPosition(pos);}
    void setCurrentSequenceIndex(unsigned int index) {sprite.setCurrentSequenceIndex(index);}
    void setColor(sf::Color color) {sprite.setColor(color);}
    void setLayer(int i) {sprite.setLayer(i);}

    void setNoUserSSP(mySpritesheet* ptr) {ssp_nouser = ptr;}
    void setWearSSP(mySpritesheet* ptr) {ssp_wear = ptr;}
    void setCarrySSP(mySpritesheet* ptr) {ssp_carry = ptr;}

    void update();
    void draw(sf::RenderWindow *target);
    void nouser() {sprite.setSpritesheetPtr(ssp_nouser);}
    void carry() {sprite.setSpritesheetPtr(ssp_carry);}
    void grabbedBy(myHuman* whom);
    void droppedBy(myHuman* whom);
    void wearedBy(myHuman* whom);
    void unwearedBy(myHuman* whom);

    bool isUsed();

  private:
    myAnimatedSprite sprite;
    sf::Vector2i position;
    sf::Color color;
    myHuman* user;
    mySpritesheet* ssp_nouser;
    mySpritesheet* ssp_wear;
    mySpritesheet* ssp_carry;
};
#endif
