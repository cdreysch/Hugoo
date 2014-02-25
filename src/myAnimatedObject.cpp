#include "myAnimatedObject.h"

myAnimatedObject::myAnimatedObject(int px, int py, myAnimationData* ptr){
  addQuad(px,py,ptr);
}

void myAnimatedObject::updateQuadSize(unsigned int quad_index){
  int x = positions.at(quad_index).x + dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getDispositionAt(currentRectIndices.at(quad_index))->x;
  int y = positions.at(quad_index).y + dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getDispositionAt(currentRectIndices.at(quad_index))->y;
  int width = dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getRectAt(currentRectIndices.at(quad_index))->width;
  int height = dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getRectAt(currentRectIndices.at(quad_index))->height;

  sf::Vertex* quad = &m_vertices[quad_index*4];
  quad[0].position = sf::Vector2f(x, y);
  quad[1].position = sf::Vector2f(x+width, y);
  quad[2].position = sf::Vector2f(x+width, y+height);
  quad[3].position = sf::Vector2f(x,  y+height);
}

void myAnimatedObject::updateQuadTexture(unsigned int quad_index){
  int tx = dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getRectAt(currentRectIndices.at(quad_index))->left;
  int ty = dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getRectAt(currentRectIndices.at(quad_index))->top;
  int width = dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getRectAt(currentRectIndices.at(quad_index))->width;
  int height = dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getRectAt(currentRectIndices.at(quad_index))->height;

  sf::Vertex* quad = &m_vertices[quad_index*4];
  quad[0].texCoords = sf::Vector2f(tx, ty);
  quad[1].texCoords =  sf::Vector2f(tx+width, ty);
  quad[2].texCoords =  sf::Vector2f(tx+width, ty+height);
  quad[3].texCoords = sf::Vector2f(tx,  ty+height);
}

void myAnimatedObject::setQuadColor(unsigned int quad_index, sf::Color color){	
  sf::Vertex* quad = &m_vertices[quad_index*4];
  quad[0].color = color;
  quad[1].color = color;
  quad[2].color = color;
  quad[3].color = color;
}

void myAnimatedObject::updateQuad(unsigned int quad_index){			
  if(frameCounter.at(quad_index)>dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getEndFrame()){
    frameCounter.at(quad_index) = 0;			
  }
  for(unsigned int i = 0; i<dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getLength();i++){			
    if(frameCounter.at(quad_index)==dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getTriggerFrameAt(i)){				
      currentRectIndices.at(quad_index) = i;				
      updateQuadTexture(quad_index);
      moveQuad(quad_index,dataPtr.at(quad_index)->getSequencePtr(currentSequenceIndices.at(quad_index))->getMoveAt(i));				
    }
  }	
  frameCounter.at(quad_index) += 1;				
}

void myAnimatedObject::update(){
  for(unsigned int i=0;i<dataPtr.size();i++){
    updateQuad(i);		
  }
}

void myAnimatedObject::moveQuad(unsigned int quad_index, sf::Vector2f* mov){
  positions.at(quad_index) += *mov;
  updateQuadSize(quad_index);
}

void myAnimatedObject::changeQuadSequence(unsigned int quad_index, unsigned int sequence_index){
  currentRectIndices.at(quad_index) = 0;
  frameCounter.at(quad_index) = 0;
  currentSequenceIndices.at(quad_index) = sequence_index;
}

void myAnimatedObject::changeSequence(unsigned int sequence_index){
  for(unsigned int i=0;i<dataPtr.size();i++){
    changeQuadSequence(i,sequence_index);	
  }		
}

void myAnimatedObject::addQuad(int px, int py, myAnimationData* ptr) {
  dataPtr.emplace_back(ptr);
  positions.emplace_back(sf::Vector2f(px,py));	
  currentRectIndices.emplace_back(0);
  currentSequenceIndices.emplace_back(0);
  frameCounter.emplace_back(0);		

  m_vertices.resize(dataPtr.size()*4);

  updateQuadSize((int)dataPtr.size()-1);
  updateQuadTexture((int)dataPtr.size()-1);	
}
