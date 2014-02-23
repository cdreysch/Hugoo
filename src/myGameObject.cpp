#include "myGameObject.h"

void myGameObject::setInterruptTable(std::vector<bool> bits){
    interruptTable = bits;
}

bool myGameObject::isInterruptable(int curStatus,int newStatus){
    return interruptTable.at(curStatus*availableStatusPtr.size()+newStatus);
}

void myGameObject::addAvailableStatusPtr(myObjectStatus* ptr){		
    availableStatusPtr.push_back(ptr);
}

myObjectStatus* myGameObject::getCurrentStatusPtr(){		
    return availableStatusPtr.at(currentStatus);
}

void myGameObject::changeStatus(int number){		
    currentStatus = number;
    getCurrentStatusPtr()->restart();
}	

void myGameObject::setIdleStatusNumber(int number){		
    idleStatusNumber = number;
}	

bool myGameObject::isIdle(){
    return currentStatus == idleStatusNumber;
}

void myGameObject::setIdle(){
    changeStatus(idleStatusNumber);
}

void myGameObject::setStatus(int number){
    if(isInterruptable(currentStatus,number))
	changeStatus(number);
}

void myGameObject::update() {		
    if(getCurrentStatusPtr()->tick()){			
	setTextureRect(getCurrentStatusPtr()->getCurrentActionTrigger().textureRect);			
	move(getCurrentStatusPtr()->getCurrentActionTrigger().moveVector);				
    }
    if(getCurrentStatusPtr()->hasEnded())
	setIdle();
}
