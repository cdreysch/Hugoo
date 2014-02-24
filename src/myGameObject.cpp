#include "myGameObject.h"

void myGameObject::setInterruptTable(std::vector<bool> bits){
    interruptTable = bits;
}

bool myGameObject::isInterruptable(int curStatus,int newStatus){
    return interruptTable.at(curStatus*availableStatus.size()+newStatus);
}

void myGameObject::addAvailableStatus(myObjectStatus os){		
    availableStatus.push_back(os);
}

void myGameObject::setAvailableStatus(std::vector<myObjectStatus> aos){
    availableStatus = aos;
}

myObjectStatus* myGameObject::getCurrentStatusPtr(){		
    return &availableStatus.at(currentStatus);
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
