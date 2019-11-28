#ifndef mySpriteManager_h
#define mySpriteManager_h mySpriteManager_h

#include <vector>
#include <algorithm>

class mySprite;

bool compare_sprite_layers(mySprite* a,mySprite* b);

class mySpriteManager {
  public:
    
    void insert(mySprite* ptr);
    void sort();
    void draw(sf::RenderWindow* target);
  private:
    std::vector<mySprite*> spritePtr;
};

#endif
