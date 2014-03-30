#include "myHuman.h"

myHuman::myHuman(int x,int y, mySpritesheet* ptr){	
  const bool bits[] = {1,1,1,1, 1,0,0,0, 1,0,0,0, 1,0,0,0};
  interruptionTable.insert(interruptionTable.begin(), bits, bits + sizeof(bits)/sizeof(bits[0]));		
  body = myAnimatedSprite(x,y,ptr);
  currentStatus=0;		
  body.setCurrentTextureIndex(1);
  slot_legs = NULL;
  slot_carry = NULL;
}
void myHuman::use(myItem* item){

}

void myHuman::grab(myItem* item){
  if(!isCarrying()) {
    body.setCurrentTextureIndex(2);
    if(hasPants()) 
      slot_legs->setCurrentTextureIndex(2);
    slot_carry = item;
    slot_carry->grabbedBy(this);
  }
}

void myHuman::drop(){
  if(isCarrying()) {
    body.setCurrentTextureIndex(1);
    if(hasPants()) 
      slot_legs->setCurrentTextureIndex(1);
    slot_carry->droppedBy(this);
    slot_carry = NULL;
  }
}

void myHuman::wear(myItem* item){
  if(!hasPants()) {
    slot_legs = item;
    slot_legs->wearedBy(this);
  }
}

/*void myHuman::wear(){
  if(isCarrying()&&!hasPants()){
    body.setCurrentTextureIndex(1);
    slot_carry->droppedBy(this);
    slot_legs = slot_carry;
    slot_legs->wearedBy(this);
    slot_carry = NULL;
  }
}*/

void myHuman::unwear(){
  if(hasPants()) {
    slot_legs->unwearedBy(this);
    slot_legs = NULL;
  }
}

void myHuman::update(){
  body.update();
  if(body.hasEnded())
    beIdle();
}

void myHuman::setCurrentStatus(unsigned int index){
  if (isInterruptable(currentStatus, index)){
    currentStatus = index;	
    body.setCurrentSequenceIndex(index);
    if(hasPants())
      slot_legs->setCurrentSequenceIndex(index);		
    if(isCarrying())
      slot_carry->setCurrentSequenceIndex(index);		
  }
}
bool myHuman::hasPants(){
  if(slot_legs==NULL) return false;
  else return true;
}

bool myHuman::isCarrying(){
  if(slot_carry==NULL) return false;
  else return true;
}
