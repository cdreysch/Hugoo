#ifndef myAnimatedSprite_h
#define myAnimatedSprite_h myAnimatedSprite_h
#include "mySprite.h"
#include "mySequence.h"
#include "mySpritesheet.h"

class myAnimatedSprite :public mySprite {
  public:
    myAnimatedSprite()								{}
    myAnimatedSprite(int x, int y, mySpritesheet* ptr);

    void update();
    bool hasEnded();
    void setCurrentSequenceIndex(unsigned int index);
    unsigned int getCurrentSequenceIndex()		{return currentSequenceIndex;}

  private:
    unsigned int frameCounter;
    unsigned int currentSequenceIndex;
};
#endif
