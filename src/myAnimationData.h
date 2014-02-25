#ifndef myAnimationData_h
#define myAnimationData_h myAnimationData_h
#include "mySequence.h"

class myAnimationData {

  private:
    std::vector<sf::Texture> textures;	
    std::vector<mySequence> availableSequences;		

  public:
    myAnimationData(std::string filename);

    void addSequence(mySequence &seq);
    void addTexture(std::string filename);

    sf::Texture* 	getTexturePtrAt(unsigned int index)	{return &textures.at(index);}
    unsigned int 	getTexturesLength()			{return textures.size();}
    mySequence* 	getSequencePtr(unsigned int index)	{return &availableSequences.at(index);}

};

#endif
