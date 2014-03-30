#ifndef mySprite_h
#define mySprite_h mySprite_h
#include <SFML/Graphics.hpp>
#include <vector>
#include "mySpritesheet.h"

class mySprite :public sf::Drawable, public sf::Transformable {
  public:
    mySprite()								{}
    mySprite(int x, int y, mySpritesheet* ptr);

    void setSpritesheetPtr(mySpritesheet* ptr)	{spriteSheetPtr = ptr;}
    void setPosition(int x, int y);
    void setPosition(sf::Vector2i pos) {setPosition(pos.x,pos.y);}
    void setColor(sf::Color color);
    void setCurrentTextureIndex(unsigned int index)	{currentTextureIndex = index;}
    void setCurrentRectIndex(unsigned int index){
      currentRectIndex = index; 
      setQuadPosition(); 
      setQuadTexCoords();
    }
    void move(sf::Vector2i dir)						{setPosition(position.x+dir.x,position.y+dir.y);}

    sf::Vector2i getDrawingPosition();
    sf::Vector2i getPosition()						{return position;}
    sf::Color getColor()							{return color;}
    unsigned int getCurrentTextureIndex()			{return currentTextureIndex;}
    unsigned int getCurrentRectIndex()				{return currentRectIndex;}
    mySpritesheet* getSpritesheetPtr() 				{return spriteSheetPtr;}

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
      states.transform *= getTransform();	
      states.texture = spriteSheetPtr->getTexturePtrAt(currentTextureIndex); 
      target.draw(&quad[0],4,sf::Quads, states);  
    }

  protected:
    void setQuadTexCoords();
    void setQuadPosition();
    void setQuadColor();
    mySpritesheet* spriteSheetPtr;
    sf::VertexArray quad;
    sf::Color color;
    sf::Vector2i position;	
    unsigned int currentTextureIndex;
    unsigned int currentRectIndex;
};


#endif
