#include "myAnimatedObject.h"

myAnimatedObject::myAnimatedObject(int px, int py, myAnimationData* ptr){
  addQuad(px,py,ptr);
  setActiveTextureIndex(0);
}
myAnimatedObject::myAnimatedObject(){
}

void myAnimatedObject::updateQuadSize(){
  int x = position.x + dataPtr->getSequencePtr(currentSequenceIndex)->getDispositionAt(currentRectIndex)->x;
  int y = position.y + dataPtr->getSequencePtr(currentSequenceIndex)->getDispositionAt(currentRectIndex)->y;
  int width = dataPtr->getSequencePtr(currentSequenceIndex)->getRectAt(currentRectIndex)->width;
  int height = dataPtr->getSequencePtr(currentSequenceIndex)->getRectAt(currentRectIndex)->height;

  sf::Vertex* quad = &m_vertices[0];
  quad[0].position = sf::Vector2f(x, y);
  quad[1].position = sf::Vector2f(x+width, y);
  quad[2].position = sf::Vector2f(x+width, y+height);
  quad[3].position = sf::Vector2f(x,  y+height);
}

void myAnimatedObject::updateQuadTexture(){
  int tx = dataPtr->getSequencePtr(currentSequenceIndex)->getRectAt(currentRectIndex)->left;
  int ty = dataPtr->getSequencePtr(currentSequenceIndex)->getRectAt(currentRectIndex)->top;
  int width = dataPtr->getSequencePtr(currentSequenceIndex)->getRectAt(currentRectIndex)->width;
  int height = dataPtr->getSequencePtr(currentSequenceIndex)->getRectAt(currentRectIndex)->height;

  sf::Vertex* quad = &m_vertices[0];
  quad[0].texCoords = sf::Vector2f(tx, ty);
  quad[1].texCoords =  sf::Vector2f(tx+width, ty);
  quad[2].texCoords =  sf::Vector2f(tx+width, ty+height);
  quad[3].texCoords = sf::Vector2f(tx,  ty+height);
}

void myAnimatedObject::setQuadColor(sf::Color color){	
  sf::Vertex* quad = &m_vertices[0];
  quad[0].color = color;
  quad[1].color = color;
  quad[2].color = color;
  quad[3].color = color;
}

void myAnimatedObject::update(){			
  if(hasEnded()){
    frameCounter = 0;			
  }
  for(unsigned int i = 0; i<dataPtr->getSequencePtr(currentSequenceIndex)->getLength();i++){			
    if(frameCounter==dataPtr->getSequencePtr(currentSequenceIndex)->getTriggerFrameAt(i)){				
      currentRectIndex = i;				
      updateQuadTexture();
      move(dataPtr->getSequencePtr(currentSequenceIndex)->getMoveAt(i));				
    }
  }	
  frameCounter += 1;				
}
bool myAnimatedObject::hasEnded(){
	return frameCounter>dataPtr->getSequencePtr(currentSequenceIndex)->getEndFrame();
}
void myAnimatedObject::move(sf::Vector2f* mov){
  position += *mov;
  updateQuadSize();
}

void myAnimatedObject::changeSequence(unsigned int sequence_index){
  currentRectIndex = 0;
  frameCounter = 0;
  currentSequenceIndex = sequence_index;
}

void myAnimatedObject::addQuad(int px, int py, myAnimationData* ptr) {
  dataPtr = ptr;
  position = sf::Vector2f(px,py);	
  currentRectIndex = 0;
  currentSequenceIndex = 0;
  frameCounter = 0;		

  m_vertices.resize(4);

  updateQuadSize();
  updateQuadTexture();	
}

void myAnimatedObject::setActiveTextureIndex(unsigned int index){
  if (index < dataPtr->getTexturesLength()){
    activeTextureIndex = index;
  }
}
