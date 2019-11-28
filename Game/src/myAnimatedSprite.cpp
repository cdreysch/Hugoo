#include "myAnimatedSprite.h"

myAnimatedSprite::myAnimatedSprite(int x, int y, mySpritesheet* ptr){
  frameCounter = 0;
  quad.resize(4);	
  position = sf::Vector2i(x,y);	
  setSpritesheetPtr(ptr); 			
  setColor(sf::Color(255,255,255,255));	
  setCurrentTextureIndex(0);		
  setCurrentRectIndex(0);	
  currentSequenceIndex = 0;
}

void myAnimatedSprite::setCurrentSequenceIndex(unsigned int index){
  currentSequenceIndex = index;
  frameCounter=0;
}

void myAnimatedSprite::update(){			
  if(hasEnded()){
    frameCounter = 0;			
  }
  for(unsigned int i = 0; i<spriteSheetPtr->getSequencePtrAt(currentSequenceIndex)->getLength();i++){			
    if(frameCounter==spriteSheetPtr->getSequencePtrAt(currentSequenceIndex)->getTriggerFrameAt(i)){				
      setCurrentRectIndex(spriteSheetPtr->getSequencePtrAt(currentSequenceIndex)->getRectIndexAt(i));
      move(*spriteSheetPtr->getSequencePtrAt(currentSequenceIndex)->getMoveAt(i));				
    }
  }	
  frameCounter += 1;				
}
bool myAnimatedSprite::hasEnded(){
  return frameCounter>spriteSheetPtr->getSequencePtrAt(currentSequenceIndex)->getEndFrame();
}
