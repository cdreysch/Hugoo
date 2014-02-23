#include "myObjectStatus.h"

void myObjectStatus::addActionTrigger(int trigFrame, sf::IntRect textRect, sf::Vector2f moveVec){
    actionTrigger newAT = { trigFrame, textRect, moveVec };
    actionTriggerStack.emplace_back(newAT);
}

actionTrigger myObjectStatus::getCurrentActionTrigger() {
    return actionTriggerStack.at(currentTriggerIndex);
}

actionTrigger myObjectStatus::getNextActionTrigger() {
    actionTrigger nextAT;
    if(currentTriggerIndex<(int)actionTriggerStack.size()-1 && currentTriggerIndex!=0)
	nextAT = actionTriggerStack.at(currentTriggerIndex+1);
    else 			
	nextAT = actionTriggerStack.at(0);
    return nextAT;
}

bool myObjectStatus::hasEnded(){		
    return (currentStatusFrame>totalStatusFrames);
}

void myObjectStatus::restart(){
    currentTriggerIndex = 0;
    currentStatusFrame = 0;
}

int myObjectStatus::tick(){
    if(hasEnded()) 
	return 0;
    else {
	currentStatusFrame++;				
	if(currentStatusFrame==getNextActionTrigger().triggerFrame){			
	    currentTriggerIndex++;				
	    return 1;		
	}else 
	    return 0;
    }		
}	

myObjectStatus::myObjectStatus(sf::IntRect textureRect,int totalFrames){
    totalStatusFrames = totalFrames;		
    currentStatusFrame = 0;
    currentTriggerIndex = 0;
    addActionTrigger(1, textureRect, sf::Vector2f(0.f,0.f));		
}
