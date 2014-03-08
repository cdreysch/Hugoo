#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include "myTileMap.h"
#include "myAnimatedObject.h"

#define mainpath_win "C:/Users/cdreysch/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_win

class mySpritesheet {
public:
	mySpritesheet()									{}
	mySpritesheet(std::string filename)				{addTexture(filename);}

	bool addTexture(std::string filename);
	void addRect(int x, int y, int width, int height, int factor=1, int xoff=0, int yoff=0);

	sf::Texture* getTexturePtrAt(unsigned int index)	{return &textures.at(index);}
	sf::IntRect* getTilePtrAt(unsigned int index)		{return &tiles.at(index);}
	sf::Vector2i* getOffsetPtrAt(unsigned int index)	{return &offsets.at(index);}
	unsigned int size()									{return offsets.size();}

private:
	std::vector<sf::Texture> textures;
	std::vector<sf::IntRect> tiles;
	std::vector<sf::Vector2i> offsets;
};

bool mySpritesheet::addTexture(std::string filename){
	sf::Image srcImage;			
	if (!srcImage.loadFromFile(filename)){
		std::cout << "Datei nicht gefunden: " << filename<< std::endl;
		return 0;
	}	
	srcImage.createMaskFromColor(sf::Color(0,0,0));	
	sf::Texture texture;
	if (!texture.loadFromImage(srcImage)){
		return 0;
	}
	textures.push_back(texture);
	return 1;
}

void mySpritesheet::addRect(int x, int y, int width, int height, int factor, int xoff, int yoff){
	tiles.emplace_back(sf::IntRect(x*factor,y*factor,width*factor,height*factor));
	offsets.emplace_back(sf::Vector2f(xoff*factor,yoff*factor));	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
class mySprite :public sf::Drawable, public sf::Transformable {
public:
	mySprite()								{}
	mySprite(int x, int y, mySpritesheet* ptr);
		
	void setTiledTexturePtr(mySpritesheet* ptr)	{tiledTexturePtr = ptr;}
	void setPosition(int x, int y);
	void setColor(sf::Color color);
	void setCurrentTextureIndex(unsigned int index)	{currentTextureIndex = index;}
	void setCurrentRectIndex(unsigned int index)	{currentRectIndex = index; setQuadPosition(); setQuadTexCoords();}
	void move(sf::Vector2i dir)						{setPosition(position.x+dir.x,position.y+dir.y);}

	sf::Vector2i getDrawingPosition();
	sf::Vector2i getPosition()						{return position;}
	sf::Color getColor()							{return color;}
	unsigned int getCurrentTextureIndex()			{return currentTextureIndex;}
	unsigned int getCurrentRectIndex()				{return currentRectIndex;}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
		states.transform *= getTransform();	
		states.texture = tiledTexturePtr->getTexturePtrAt(currentTextureIndex); 
		target.draw(&quad[0],4,sf::Quads, states);  
	}

protected:
	void setQuadTexCoords();
	void setQuadPosition();
	void setQuadColor();
	mySpritesheet* tiledTexturePtr;
	sf::VertexArray quad;
	sf::Color color;
	sf::Vector2i position;	
	unsigned int currentTextureIndex;
	unsigned int currentRectIndex;
};

mySprite::mySprite(int x, int y, mySpritesheet* ptr){
	quad.resize(4);	
	position = sf::Vector2i(x,y);	
	setTiledTexturePtr(ptr); 			
	setColor(sf::Color(255,255,255,255));	
	setCurrentTextureIndex(0);		
	setCurrentRectIndex(0);	
}

sf::Vector2i mySprite::getDrawingPosition(){		
	int xd = position.x+tiledTexturePtr->getOffsetPtrAt(currentRectIndex)->x;
	int yd = position.y+tiledTexturePtr->getOffsetPtrAt(currentRectIndex)->y;	
	
	return sf::Vector2i(xd,yd);
}

void mySprite::setPosition(int x, int y){
	position = sf::Vector2i(x,y);	
	setQuadPosition();
}

void mySprite::setColor(sf::Color newcolor){
	color = newcolor;
	setQuadColor();
}

void mySprite::setQuadTexCoords(){	
	int tx = tiledTexturePtr->getTilePtrAt(currentRectIndex)->left;
	int ty = tiledTexturePtr->getTilePtrAt(currentRectIndex)->top;
	int width = tiledTexturePtr->getTilePtrAt(currentRectIndex)->width;
	int height = tiledTexturePtr->getTilePtrAt(currentRectIndex)->height;

	quad[0].texCoords = sf::Vector2f(tx, ty);
	quad[1].texCoords =  sf::Vector2f(tx+width, ty);
	quad[2].texCoords =  sf::Vector2f(tx+width, ty+height);
	quad[3].texCoords = sf::Vector2f(tx,  ty+height);
}

void mySprite::setQuadPosition(){	
	int x = getDrawingPosition().x;
	int y = getDrawingPosition().y;
	int width = tiledTexturePtr->getTilePtrAt(currentRectIndex)->width;
	int height =tiledTexturePtr->getTilePtrAt(currentRectIndex)->height;

	if (width<0)
		width *= -1;

	if (height<0)
		height *= -1;

	quad[0].position = sf::Vector2f(x, y);
	quad[1].position = sf::Vector2f(x+width, y);
	quad[2].position = sf::Vector2f(x+width, y+height);
	quad[3].position = sf::Vector2f(x,  y+height);
}

void mySprite::setQuadColor(){
	quad[0].color = color;
	quad[1].color = color;
	quad[2].color = color;
	quad[3].color = color;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
class myAnimatedSprite :public mySprite{
public:
	myAnimatedSprite()									{};
	myAnimatedSprite(int x, int y, mySpritesheet* ptr);

	void update();
	void addSequencePtr(mySequence* ptr);
	void setCurrentSequenceIndex(unsigned int index);

	unsigned int getCurrentSequenceIndex()				{return currentSequenceIndex;}

	bool hasEnded();

private:
	std::vector<mySequence*> seqPtr;
	unsigned int currentSequenceIndex;
	unsigned long frameCounter;
};
myAnimatedSprite::myAnimatedSprite(int x, int y, mySpritesheet* ptr){	
	quad.resize(4);	
	position = sf::Vector2i(x,y);	
	setTiledTexturePtr(ptr); 			
	setColor(sf::Color(255,255,255,255));	
	setCurrentTextureIndex(0);		
	setCurrentRectIndex(0);		
	frameCounter = 0;
	currentSequenceIndex = 0;
}

void myAnimatedSprite::addSequencePtr(mySequence* ptr){
	seqPtr.emplace_back(ptr);
}

void myAnimatedSprite::setCurrentSequenceIndex(unsigned int index){	
	frameCounter = 0;
	currentSequenceIndex = index;
	setCurrentRectIndex(0);		
}

bool myAnimatedSprite::hasEnded(){
	return frameCounter>seqPtr.at(currentSequenceIndex)->getEndFrame();
}

void myAnimatedSprite::update(){
	if(hasEnded()){
		frameCounter = 0;			
	}	
	for(unsigned int i = 0; i<seqPtr.at(currentSequenceIndex)->getLength();i++){		
		if(frameCounter==seqPtr.at(currentSequenceIndex)->getTriggerFrameAt(i)){			
			setCurrentRectIndex(seqPtr.at(currentSequenceIndex)->getRectIndexAt(i));			
			move(*seqPtr.at(currentSequenceIndex)->getMoveAt(i));				
		}
	}	
	frameCounter += 1;	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
class myTree {
public:
	myTree(int x,int y, myAnimationData* woodDataPtr, myAnimationData* leafsDataPtr){	
		const bool bits[] = {1,1, 1,0};
		interruptionTable.insert(interruptionTable.begin(), bits, bits + sizeof(bits)/sizeof(bits[0]));		
		wood = myAnimatedObject(x,y,woodDataPtr);
		leafs = myAnimatedObject(x,y,leafsDataPtr);	
		color = sf::Color(55,155,55,255);
		leafs.setQuadColor(color);
		currentStatus=0;		
		leafs.setActiveTextureIndex(0);
		isDead = 0;
		ticks= 0L;
		grow();
	}

	void draw(sf::RenderWindow *target){
		target->draw(wood);
		if (!isDead)
			target->draw(leafs);
	}
	void update(){
		ticks++;
		if (ticks%50 == 0){
			color += sf::Color(1,0,0,-10);
			if (ticks>200*50) {
				ticks = 0L;
				toggleDead();
			}
			leafs.setQuadColor(color);
		}
		wood.update();
		leafs.update();
		if(wood.hasEnded())
			beIdle();
	}
	void grow(){	  
		setCurrentStatus(1);      
	}	
	void beIdle(){			
		setCurrentStatus(0);		
	}

	void setCurrentStatus(unsigned int index){
		if (isInterruptable(currentStatus, index)){
			currentStatus = index;	
			wood.changeSequence(index);
			leafs.changeSequence(index);		
		}
	}
	void toggleDead(){
		isDead = !isDead;
		color = sf::Color(55,155,55,255);
		std::cout << isDead << std::endl;		
	}
	bool isIdle(){
		return wood.getSequenceIndex()==0;
	}
	bool isInterruptable(int curStatus,int newStatus){		
		return interruptionTable.at(curStatus*2+newStatus);
	}
	sf::Vector2f getPosition(){
		position = wood.getPosition();
		return position;
	}


private:
	std::vector<bool> interruptionTable;
	sf::Vector2f position;
	myAnimatedObject wood;
	myAnimatedObject leafs;	
	unsigned int currentStatus;
	bool isDead;
	sf::Color color;
	unsigned long ticks;
};

class myHuman {
public:
	myHuman(int x,int y, myAnimationData* bodyDataPtr, myAnimationData* hoseDataPtr){	
		const bool bits[] = {1,1,1,1, 1,0,0,0, 1,0,0,0, 1,0,0,0};
		interruptionTable.insert(interruptionTable.begin(), bits, bits + sizeof(bits)/sizeof(bits[0]));		
		body = myAnimatedObject(x,y,bodyDataPtr);
		hose = myAnimatedObject(x,y,hoseDataPtr);	
		hose.setQuadColor(sf::Color(155,55,55,255));
		currentStatus=0;		
		body.setActiveTextureIndex(1);
	}

	void draw(sf::RenderWindow *target){
		target->draw(body);
		target->draw(hose);
	}
	void update(){
		body.update();
		hose.update();
		if(body.hasEnded())
			beIdle();
	}
	void moveRight(){	  
		setCurrentStatus(2);      
	}
	void moveLeft(){	  
		setCurrentStatus(3);      
	}
	void beIdle(){			
		setCurrentStatus(0);		
	}
	void plant(){      
		setCurrentStatus(1);
	}
	void setCurrentStatus(unsigned int index){
		if (isInterruptable(currentStatus, index)){
			currentStatus = index;	
			body.changeSequence(index);
			hose.changeSequence(index);		
		}
	}
	bool isIdle(){
		return body.getSequenceIndex()==0;
	}
	bool isInterruptable(int curStatus,int newStatus){		
		return interruptionTable.at(curStatus*4+newStatus);
	}
	sf::Vector2f getPosition(){
		position = body.getPosition();
		return position;
	}


private:
	std::vector<bool> interruptionTable;
	sf::Vector2f position;
	myAnimatedObject body;
	myAnimatedObject hose;	
	unsigned int currentStatus;
};

int main()
{
	srand (time(NULL));

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// VideoModes
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	int fullscreenModeIndex=0;	
	float fullscreenRatio = (float)modes[fullscreenModeIndex].width/(float)modes[fullscreenModeIndex].height;
	sf::VideoMode fullscreenMode = modes[fullscreenModeIndex];	

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Window
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	int window_width = 640;
	int wondow_height = (int)(640.f/fullscreenRatio);
	sf::VideoMode windowedMode = sf::VideoMode(window_width, wondow_height);
	sf::RenderWindow window(windowedMode, "Hugoo");	

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// View
	/////////////////////////////////////////////////////////////////////////////////////////////////////	

	float zoom = 2.f;
	sf::View view;	
	view.reset(sf::FloatRect(0, 0, window_width/zoom, (float)window_width/(zoom*fullscreenRatio)));
	bool toggleViewFollowHugoo = true;
	bool isFullscreen = false;
	window.setView(view);		

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// FPS Display
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	sf::Int64 totalFrames = 0;
	sf::Int32 elapsedFrames = 0;
	sf::Int32 fpsUpdateFrequency = 10; //Hertz
	sf::Int32 frameratelimit = 50;
	sf::Clock clock;

	window.setFramerateLimit(frameratelimit);

	sf::Font font;
	if (!font.loadFromFile(std::string("src/fonts/FreeMono.ttf").insert(0,mainpath))){
		// error...
	}
	sf::Text text;
	text.setFont(font); // font is a sf::Font
	text.setCharacterSize(12); // in pixels, not points!
	text.setColor(sf::Color(50,50,50));

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// General
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	int tilesize = 16;
	sf::Color bgColor = sf::Color(176,228,252);
	sf::Color alphaColor = sf::Color(0,0,0);		

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// World
	/////////////////////////////////////////////////////////////////////////////////////////////////////	

	int world_width = 320;
	int world_height = 80;

	std::vector<int> world; 

	for (int iy=0;iy<world_height;iy++){
		for (int ix=0;ix<world_width;ix++){			
			if(iy<15){
				world.emplace_back(0);			
			}else if(iy==15){
				world.emplace_back(3);
			} else if(iy>15){
				world.emplace_back(40);
			}			
		}
	}	

	myTileMap map;
	if (!map.load(std::string("src/textures/minecraft_tiles16.png").insert(0,mainpath), sf::Vector2u(tilesize, tilesize), &world, world_width, world_height))
		return -1;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game Objects
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////// Hugoo //////////////////////////////////////////////////////////


#include "hugooStatus.cfg"

	myAnimationData hugooData = myAnimationData(std::string("src/textures/huNaked1.png").insert(0,mainpath));
	hugooData.addTexture(std::string("src/textures/huNaked2.png").insert(0,mainpath));
	hugooData.addSequence(seq0);
	hugooData.addSequence(seq1);
	hugooData.addSequence(seq2);
	hugooData.addSequence(seq3);	

	myAnimationData huHoseData = myAnimationData(std::string("src/textures/huHose1.png").insert(0,mainpath));
	huHoseData.addSequence(seq0);
	huHoseData.addSequence(seq1);
	huHoseData.addSequence(seq2);
	huHoseData.addSequence(seq3);

	myHuman hugoo = myHuman(12*tilesize,14*tilesize,&hugooData,&huHoseData);

	////////////////////////////// Tree //////////////////////////////////////////////////////////

#include "treeStatus.cfg"

	myAnimationData treeWoodData = myAnimationData(std::string("src/textures/treeWood.png").insert(0,mainpath));	
	treeWoodData.addSequence(treeSeq0);
	treeWoodData.addSequence(treeSeq1);

	myAnimationData treeLeafsData = myAnimationData(std::string("src/textures/treeLeafsW.png").insert(0,mainpath));	
	treeLeafsData.addSequence(treeSeq0);
	treeLeafsData.addSequence(treeSeq1);		

	std::vector<myTree> trees;
	trees.emplace_back(myTree(16*tilesize,14*tilesize,&treeWoodData,&treeLeafsData));

	/////////////////////////// Other ///////////////////////////////////////////////////////////////
	mySpritesheet treeData = mySpritesheet(std::string("src/textures/treeWood.png").insert(0,mainpath));
	treeData.addTexture(std::string("src/textures/treeLeafsW.png").insert(0,mainpath));
	treeData.addRect(0,0,1,1,tilesize);
	treeData.addRect(0,1,1,1,tilesize);
	treeData.addRect(0,2,1,1,tilesize);
	treeData.addRect(0,3,1,2,tilesize,0,-1);
	treeData.addRect(1,1,3,4,tilesize,-1,-3);
	treeData.addRect(4,0,5,5,tilesize,-2,-4);
		
	mySequence treeSeq1a = mySequence("Growing",600);
	treeSeq1a.addEntry(0,	0,	0,0);
	treeSeq1a.addEntry(100,	1,	0,0);
	treeSeq1a.addEntry(200,	2,	0,0);
	treeSeq1a.addEntry(300,	3,	0,0);
	treeSeq1a.addEntry(400,	4,	0,0);
	treeSeq1a.addEntry(500,	5,	0,0);

	mySequence treeSeq0a = mySequence("Standing",2000);
	treeSeq0a.addEntry(0,	6,	0,0);

	mySprite staticTree = mySprite(10*tilesize,14*tilesize,&treeData);
	staticTree.setCurrentRectIndex(4);

	myAnimatedSprite dynTree = myAnimatedSprite(8*tilesize,14*tilesize,&treeData);	
	dynTree.addSequencePtr(&treeSeq0a);	
	dynTree.addSequencePtr(&treeSeq1a);	
	dynTree.setCurrentSequenceIndex(1);
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game Loop
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	view.setCenter(hugoo.getPosition()+sf::Vector2f(0.f,-32.f));
	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed) { // Close window : exit
				window.close();
			}
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Escape: // Escape pressed : exit
					window.close();
					break;
				case sf::Keyboard::L: // l pressed : Hugoo move right! 										
					hugoo.moveRight();
					break;
				case sf::Keyboard::H: // h pressed : Hugoo move left! 					
					hugoo.moveLeft();
					break;
				case sf::Keyboard::J: // j pressed : Hugoo stop moving! 					
					hugoo.beIdle();
					break;
				case sf::Keyboard::F: // F pressed : Hugoo do something!									
					hugoo.plant();
					/*if(hugoo.getPosition()==tree1.getPosition())
					tree1.toggleDead();
					else
					map.changeTile(hugoo.getPosition(),(rand() % 2) + 12 + (rand() % 2)*16);*/
					trees.emplace_back(myTree(hugoo.getPosition().x,hugoo.getPosition().y,&treeWoodData,&treeLeafsData));					
					break;
				case sf::Keyboard::T: // T pressed : toggle following					
					toggleViewFollowHugoo = !toggleViewFollowHugoo;
					break;
				case sf::Keyboard::W:
					toggleViewFollowHugoo = false;
					view.move(0.f,-8.f);
					break;
				case sf::Keyboard::S: 
					toggleViewFollowHugoo = false;
					view.move(0.f,8.f);
					break;
				case sf::Keyboard::A: 
					toggleViewFollowHugoo = false;
					view.move(-8.f,0.f);
					break;
				case sf::Keyboard::D:
					toggleViewFollowHugoo = false;					
					view.move(8.f,0.f);			
					break;
				case sf::Keyboard::F11: // F11 pressed: toggle fullscreen 
					if (!isFullscreen) {
						window.close();
						window.create(fullscreenMode,"Hugoo",sf::Style::Fullscreen);
						window.setFramerateLimit(frameratelimit);
						isFullscreen = true;						
					}else {
						window.close();
						window.create(windowedMode,"Hugoo");
						window.setFramerateLimit(frameratelimit);
						isFullscreen = false;
					}
					break;
				default:					
					break;
				}
			} else if(event.type == sf::Event::KeyReleased) {
				std::cout << "keycode: " << event.key.code << std::endl;
			}
		}
		window.clear(bgColor);
		if(toggleViewFollowHugoo){view.setCenter(hugoo.getPosition()+sf::Vector2f(0.f,-32.f));}
		window.setView(view);

		for (unsigned int i =0; i<trees.size();i++){
			trees.at(i).update();
			trees.at(i).draw(&window);
		}	

		window.draw(staticTree);
		dynTree.update();
		window.draw(dynTree);

		hugoo.update();
		hugoo.draw(&window);


		window.draw(map);


		totalFrames++;
		elapsedFrames++;
		sf::Time elapsedTime = clock.getElapsedTime();
		if (elapsedTime.asSeconds()>=1.f/fpsUpdateFrequency){
			std::stringstream textStream;
			textStream << "fps: " << elapsedFrames/elapsedTime.asSeconds();
			text.setString(textStream.str());
			elapsedFrames = 0;
			sf::Time elapsedTime = clock.restart();
		}
		window.setView(window.getDefaultView());
		window.draw(text);
		window.display();
	}
	return 0;
}
