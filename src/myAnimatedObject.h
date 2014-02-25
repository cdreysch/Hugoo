#ifndef myAnimatedObject_h
#define myAnimatedObject_h myAnimatedObject_h 
#include "myAnimationData.h"

class myAnimatedObject :public sf::Drawable, public sf::Transformable {
  public:
    myAnimatedObject(int px, int py, myAnimationData* ptr);
    void updateQuadSize(unsigned int quad_index);
    void updateQuadTexture(unsigned int quad_index);
    void setQuadColor(unsigned int quad_index, sf::Color color);	
    void updateQuad(unsigned int quad_index);
    void update();
    void moveQuad(unsigned int quad_index, sf::Vector2f* mov);
    void changeQuadSequence(unsigned int quad_index, unsigned int sequence_index);
    void changeSequence(unsigned int sequence_index);
    void addQuad(int px, int py, myAnimationData* ptr);

  private:
    sf::VertexArray m_vertices;	
    std::vector<sf::Vector2f> positions;		
    std::vector<myAnimationData*> dataPtr;
    std::vector<unsigned int> currentSequenceIndices;
    std::vector<unsigned int> currentRectIndices;
    std::vector<unsigned int> frameCounter;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
      states.transform *= getTransform();	
      for(unsigned int qi=0;qi<dataPtr.size();qi++){
	for(unsigned int ti=0;ti<dataPtr.at(qi)->getTexturesLength();ti++){				
	  states.texture = dataPtr.at(qi)->getTexturePtrAt(ti); 
	  target.draw(&m_vertices[qi*4],4,sf::Quads, states);  
	}
      }			
    }
};

#endif
