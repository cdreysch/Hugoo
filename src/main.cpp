#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include "myTileMap.h"
#include "myAnimatedObject.h"

#define mainpath_win "C:/Users/cdreysch/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_win

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
	treeLeafsData.addTexture(std::string("src/textures/treeLeafs.png").insert(0,mainpath));
	treeLeafsData.addSequence(treeSeq0);
	treeLeafsData.addSequence(treeSeq1);
		
	myTree tree1 = myTree(16*tilesize,14*tilesize,&treeWoodData,&treeLeafsData);
	tree1.grow();

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
					if(hugoo.getPosition()==tree1.getPosition())
						tree1.toggleDead();
					else
					map.changeTile(hugoo.getPosition(),(rand() % 2) + 12 + (rand() % 2)*16);
					
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
				
		tree1.update();
		//tree2.update();
		tree1.draw(&window);
		//window.draw(tree2);	

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
