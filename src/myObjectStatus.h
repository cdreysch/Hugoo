#ifndef myObjectStatus_h
#define myObjectStatus_h myObjectStatus_h 

#include <SFML/Graphics.hpp>
#include <vector>

struct actionTrigger {int triggerFrame; sf::IntRect textureRect; sf::Vector2f moveVector;};

class myObjectStatus {
    private:
	std::vector<actionTrigger> actionTriggerStack;
	int currentTriggerIndex;
	int totalStatusFrames;
	int currentStatusFrame;		

    public:
	void addActionTrigger(int trigFrame, sf::IntRect textRect, sf::Vector2f moveVec);
	actionTrigger getCurrentActionTrigger();
	actionTrigger getNextActionTrigger();
	bool hasEnded();
	void restart();
	int tick();
	myObjectStatus(sf::IntRect textureRect,int totalFrames);
};
#endif
