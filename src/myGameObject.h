#ifndef myGameObject_h
#define myGameObject_h myGameObject_h

#include <SFML/Graphics.hpp>
#include <vector>
#include "myObjectStatus.h"

class myGameObject :public sf::Sprite{
  private:	
    std::vector<myObjectStatus> availableStatus;
    int currentStatus;	
    int idleStatusNumber;
    std::vector<bool> interruptTable;

  public:
    void setInterruptTable(std::vector<bool> bits);
    bool isInterruptable(int curStatus,int newStatus);
    void addAvailableStatus(myObjectStatus ptr);
    void setAvailableStatus(std::vector<myObjectStatus> as);
    myObjectStatus* getCurrentStatusPtr();		
    void changeStatus(int number);		
    void setIdleStatusNumber(int number);		
    bool isIdle();
    void setIdle();
    void setStatus(int number);
    void update();		
};
#endif
