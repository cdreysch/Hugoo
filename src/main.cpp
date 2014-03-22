#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include "myTileMap.h"
#include "mySpritesheet.h"
#include "mySprite.h"
#include "myAnimatedSprite.h"

#define mainpath_win "C:/Users/cdreysch/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_unix

/*class myEquipable {

  }*/


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
class myTree {
  public:
    myTree(int x,int y, mySpritesheet* ptr){	
      const bool bits[] = {1,1, 1,0};
      interruptionTable.insert(interruptionTable.begin(), bits, bits + sizeof(bits)/sizeof(bits[0]));		
      wood = myAnimatedSprite(x,y,ptr);
      leafs = myAnimatedSprite(x,y,ptr);	
      color = sf::Color(55,155,55,255);
      leafs.setColor(color);
      currentStatus=0;		
      leafs.setCurrentTextureIndex(1);
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
	leafs.setColor(color);
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
	wood.setCurrentSequenceIndex(index);
	leafs.setCurrentSequenceIndex(index);		
      }
    }
    void toggleDead(){
      isDead = !isDead;
      color = sf::Color(55,155,55,255);
      std::cout << isDead << std::endl;		
    }
    bool isIdle(){
      return wood.getCurrentSequenceIndex()==0;
    }
    bool isInterruptable(int curStatus,int newStatus){		
      return interruptionTable.at(curStatus*2+newStatus);
    }
    sf::Vector2i getPosition(){
      position = wood.getPosition();
      return position;
    }


  private:
    std::vector<bool> interruptionTable;
    sf::Vector2i position;
    myAnimatedSprite wood;
    myAnimatedSprite leafs;	
    unsigned int currentStatus;
    bool isDead;
    sf::Color color;
    unsigned long ticks;
};

class myHuman {
  public:
    bool hathose = true;
    myHuman(int x,int y, mySpritesheet* ptr){	
      const bool bits[] = {1,1,1,1, 1,0,0,0, 1,0,0,0, 1,0,0,0};
      interruptionTable.insert(interruptionTable.begin(), bits, bits + sizeof(bits)/sizeof(bits[0]));		
      body = myAnimatedSprite(x,y,ptr);
      hose = myAnimatedSprite(x,y,ptr);
      hose.setColor(sf::Color(155,55,55,255));
      hose.setCurrentTextureIndex(2);
      currentStatus=0;		
      body.setCurrentTextureIndex(0);
    }

    void draw(sf::RenderWindow *target){
      target->draw(body);
      if (hathose)
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
	body.setCurrentSequenceIndex(index);
	hose.setCurrentSequenceIndex(index);		
      }
    }
    bool isIdle(){
      return body.getCurrentSequenceIndex()==0;
    }
    bool isInterruptable(int curStatus,int newStatus){		
      return interruptionTable.at(curStatus*4+newStatus);
    }
    sf::Vector2i getPosition(){
      position = body.getPosition();
      return position;
    }


  private:
    std::vector<bool> interruptionTable;
    sf::Vector2i position;
    myAnimatedSprite body;
    myAnimatedSprite hose;	
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
sf::Time elapsedTime = sf::Time();
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


  mySpritesheet huData = mySpritesheet(std::string("src/textures/huNaked1.png").insert(0,mainpath));
  huData.addTexture(std::string("src/textures/huNaked2.png").insert(0,mainpath));
  huData.addTexture(std::string("src/textures/huHose1.png").insert(0,mainpath));

#include "hugooStatus.cfg"

  huData.addSequencePtr(&seq0);
  huData.addSequencePtr(&seq1);
  huData.addSequencePtr(&seq2);
  huData.addSequencePtr(&seq3);	

  myHuman hugoo = myHuman(12*tilesize,14*tilesize,&huData);

  ////////////////////////////// Tree //////////////////////////////////////////////////////////

//#include "treeStatus.cfg"

  mySpritesheet treeData = mySpritesheet(std::string("src/textures/treeWood.png").insert(0,mainpath));
  treeData.addTexture(std::string("src/textures/treeLeafsW.png").insert(0,mainpath));
  treeData.defineTile(0,0,1,1,tilesize);
  treeData.defineTile(0,1,1,1,tilesize);
  treeData.defineTile(0,2,1,1,tilesize);
  treeData.defineTile(0,3,1,2,tilesize,0,-1);
  treeData.defineTile(1,1,3,4,tilesize,-1,-3);
  treeData.defineTile(4,0,5,5,tilesize,-2,-4);

  mySequence treeSeq1a = mySequence("Growing",6000);
  treeSeq1a.addEntry(0,	0,	0,0);
  treeSeq1a.addEntry(1000,	1,	0,0);
  treeSeq1a.addEntry(2000,	2,	0,0);
  treeSeq1a.addEntry(3000,	3,	0,0);
  treeSeq1a.addEntry(4000,	4,	0,0);
  treeSeq1a.addEntry(5000,	5,	0,0);

  mySequence treeSeq0a = mySequence("Standing",2000);
  treeSeq0a.addEntry(0,	5,	0,0);

  treeData.addSequencePtr(&treeSeq0a);	
  treeData.addSequencePtr(&treeSeq1a);	

  std::vector<myTree> trees;
  trees.emplace_back(myTree(16*tilesize,14*tilesize,&treeData));


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Game Loop
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  view.setCenter(hugoo.getPosition().x,hugoo.getPosition().y-32);
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
	    trees.emplace_back(myTree(hugoo.getPosition().x,hugoo.getPosition().y,&treeData));					
	    break;
	  case sf::Keyboard::T: // T pressed : toggle following					
	    toggleViewFollowHugoo = !toggleViewFollowHugoo;
	    hugoo.hathose = !hugoo.hathose;
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
	  case sf::Keyboard::F12: // F12 pressed: change framerate  
	    frameratelimit -= 10;

	    if (frameratelimit<0) {
	      frameratelimit = 50;
	      window.setFramerateLimit(frameratelimit);
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
    if(toggleViewFollowHugoo){view.setCenter(hugoo.getPosition().x,hugoo.getPosition().y-32);}
    window.setView(view);

    for (unsigned int i =0; i<trees.size();i++){
      trees.at(i).update();
      trees.at(i).draw(&window);
    }	

    hugoo.update();
    hugoo.draw(&window);


    window.draw(map);


    totalFrames++;
    elapsedFrames++;
    elapsedTime = clock.getElapsedTime();
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
