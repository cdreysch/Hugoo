#ifndef myHuman_h
#define myHuman_h myHuman_h

#include <cstddef>  //NULL
#include <SFML/Graphics.hpp>
#include "mySpritesheet.h"
#include "myAnimatedSprite.h"
#include "myItem.h"

class myHuman {
  public:
    myHuman()	{}
    myHuman(int x,int y, mySpritesheet* ptr);

    mySpritesheet* getSpritesheetPtr()			{return body.getSpritesheetPtr();}
    unsigned int getCurrentStatus() 			{return currentStatus;}
    unsigned int getCurrentTextureIndex()		{return body.getCurrentTextureIndex();}
    sf::Vector2i getPosition()				{return body.getPosition();}
    
    bool isIdle()					{return body.getCurrentSequenceIndex()==0;}
    bool isInterruptable(int curStatus,int newStatus)	{return interruptionTable.at(curStatus*4+newStatus);}
    bool hasPants();
    bool isCarrying();

    void setCurrentStatus(unsigned int index);
    void draw(sf::RenderWindow *target)			{target->draw(body);}
    void use(myItem* item);
    void grab(myItem* item);
    void drop();
    void wear(myItem* item);
    //void wear();
    void unwear();
    void update();
    void moveRight()					{setCurrentStatus(2);}      
    void moveLeft()					{setCurrentStatus(3);}      
    void beIdle()					{setCurrentStatus(0);}
    void plant()					{setCurrentStatus(1);}
/*    void carry()					{body.setCurrentTextureIndex(2);
      if(hasPants()){
	slot_legs->carry();
	slot_legs->setCurrentTextureIndex(body.getCurrentTextureIndex());
      }
    }  
    void nocarry()					{body.setCurrentTextureIndex(1);
      if(hasPants()){
	slot_legs->wear();
	slot_legs->setCurrentTextureIndex(body.getCurrentTextureIndex());
      }
    }
*/
  private:
    std::vector<bool> interruptionTable;
    sf::Vector2i position;
    myAnimatedSprite body;
    myItem* slot_legs;	
    myItem* slot_carry;
    unsigned int currentStatus;
    bool hatslot_legs;
};

#endif
