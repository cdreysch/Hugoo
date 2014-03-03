#ifndef myAnimatedObject_h
#define myAnimatedObject_h myAnimatedObject_h 
#include "myAnimationData.h"

class myAnimatedObject :public sf::Drawable, public sf::Transformable {
  public:
    myAnimatedObject(int px, int py, myAnimationData* ptr);
    myAnimatedObject();
    void updateQuadSize();
    void updateQuadTexture();
    void setQuadColor(sf::Color color);	    
    void update();
    void move(sf::Vector2f* mov);    
    void changeSequence(unsigned int sequence_index);
	unsigned int getSequenceIndex(){return currentSequenceIndex;}
    void addQuad(int px, int py, myAnimationData* ptr);
    void setActiveTextureIndex(unsigned int index);
	bool hasEnded();
	sf::Vector2f getPosition(){return position;}
	sf::Vector2f getDrawingPosition();

  private:
    sf::VertexArray m_vertices;	
    sf::Vector2f position;		
    myAnimationData* dataPtr;
    unsigned int currentSequenceIndex;
    unsigned int currentRectIndex;
    unsigned int frameCounter;
    unsigned int activeTextureIndex;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
      states.transform *= getTransform();	
//	for(unsigned int ti=0;ti<activeTextureIndices.size();ti++){				
	  states.texture = dataPtr->getTexturePtrAt(activeTextureIndex); 
	  target.draw(&m_vertices[0],4,sf::Quads, states);  
//	}
    }
};

#endif
