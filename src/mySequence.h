#ifndef mySequence_h
#define  mySequence_h mySequence_h
#include <vector>
#include <SFML/Graphics.hpp>

class mySequence {

  private:
    std::string name;
    unsigned int endFrame;
    std::vector<unsigned int> triggerFrames;
    std::vector<sf::IntRect> textureRects;
    std::vector<sf::Vector2f> textureDispositions;
    std::vector<sf::Vector2f> moves;

  public:
    void addEntry(unsigned int triggerFrame, int x, int y, int width, int height, int dispoX, int dispoY, int factor, int moveX, int moveY);

    sf::IntRect* 	getRectAt(unsigned int index)		{return &textureRects.at(index);}
    sf::Vector2f* 	getDispositionAt(unsigned int index)	{return &textureDispositions.at(index);}
    sf::Vector2f* 	getMoveAt(unsigned int index)		{return &moves.at(index);}
    unsigned int 	getTriggerFrameAt(unsigned int index)	{return triggerFrames.at(index);}
    unsigned int 	getLength()				{return textureDispositions.size();}
    unsigned int 	getEndFrame()				{return endFrame;}
    std::string* 	getName()				{return &name;}

    mySequence(std::string Name, unsigned int eframe){
      name = Name;
      endFrame = eframe;
    }
};

#endif
