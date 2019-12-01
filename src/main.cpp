#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "myTileMap.h"
#include "mySpritesheet.h"
#include "mySprite.h"
#include "myAnimatedSprite.h"
#include "myItem.h"
#include "myHuman.h"
#include "mySpriteManager.h"

#define mainpath_win "C:/Users/Chris/source/repos/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_win

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

    //mySprite* getSpritePtr() {return &sprite;}

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
	  char temp;
	  std::cin >> temp;
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
  huData.addTexture(std::string("src/textures/huCarry1.png").insert(0,mainpath));

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

  mySequence treeSeq0a = mySequence("Standing",20);
  treeSeq0a.addEntry(0,	5,	0,0);

  treeData.addSequencePtr(&treeSeq0a);	
  treeData.addSequencePtr(&treeSeq1a);	

  std::vector<myTree> trees;
  trees.emplace_back(myTree(16*tilesize,14*tilesize,&treeData));


  ////////////////////////////// Pants //////////////////////////////////////////////////////////

  mySpritesheet ss_pants1_nouser = mySpritesheet(std::string("src/textures/pants1_nouser.png").insert(0,mainpath));
  ss_pants1_nouser.defineTile(0,0,1,1,tilesize);

  mySequence seq_pants1_nouser0 = mySequence("Laying",6000);
  seq_pants1_nouser0.addEntry(0,	0,	0,0);
  ss_pants1_nouser.addSequencePtr(&seq_pants1_nouser0);

  ////////////////huWear///////////////
  mySpritesheet ss_pants1_huWear = mySpritesheet(std::string("src/textures/pants1_huWear_tex0.png").insert(0,mainpath));
  ss_pants1_huWear.addTexture(std::string("src/textures/pants1_huWear_tex1.png").insert(0,mainpath));
  ss_pants1_huWear.addTexture(std::string("src/textures/pants1_huWear_tex2.png").insert(0,mainpath));

#include "pants1_huWear.ani"

  ss_pants1_huWear.addSequencePtr(&seq_pants_huWear0);
  ss_pants1_huWear.addSequencePtr(&seq_pants_huWear1);
  ss_pants1_huWear.addSequencePtr(&seq_pants_huWear2);
  ss_pants1_huWear.addSequencePtr(&seq_pants_huWear3);

  ////////////////huCarry///////////////
  mySpritesheet ss_pants1_huCarry = mySpritesheet(std::string("src/textures/pants1_huCarry_tex0.png").insert(0,mainpath));
  ss_pants1_huCarry.addTexture(std::string("src/textures/pants1_huCarry_tex1.png").insert(0,mainpath));
  ss_pants1_huCarry.addTexture(std::string("src/textures/pants1_huCarry_tex2.png").insert(0,mainpath));

#include "pants1_huCarry.ani"

  ss_pants1_huCarry.addSequencePtr(&seq_pants_huCarry0);
  ss_pants1_huCarry.addSequencePtr(&seq_pants_huCarry1);
  ss_pants1_huCarry.addSequencePtr(&seq_pants_huCarry2);
  ss_pants1_huCarry.addSequencePtr(&seq_pants_huCarry3);

  myItem pants1 = myItem(18*tilesize,14*tilesize,&ss_pants1_nouser);
  pants1.setColor(sf::Color(50,200,50));
  pants1.setWearSSP(&ss_pants1_huWear);
  pants1.setCarrySSP(&ss_pants1_huCarry);

  myItem pants2 = myItem(28*tilesize,14*tilesize,&ss_pants1_nouser);
  pants2.setColor(sf::Color(150,80,50));
  pants2.setWearSSP(&ss_pants1_huWear);
  pants2.setCarrySSP(&ss_pants1_huCarry);

  std::vector<myItem> items;
  items.push_back(pants1);
  items.push_back(pants2);
  /*for(int i = 0; i<1000; i++) {
    items.emplace_back((28+0*i)*tilesize,14*tilesize,&ss_pants1_nouser);
    //items.at(2+i).setColor(sf::Color(250-10*i,10*i,100));
    items.at(2+i).setWearSSP(&ss_pants1_huWear);
    items.at(2+i).setCarrySSP(&ss_pants1_huCarry);
  }*/

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Sprite Manager
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  mySpriteManager spriteManager = mySpriteManager();
  spriteManager.insert(hugoo.getSpritePtr());
  spriteManager.insert(items.at(0).getSpritePtr());
  spriteManager.insert(items.at(1).getSpritePtr());
  /*for(int i = 0; i<1000; i++) {
    spriteManager.insert(items.at(2+i).getSpritePtr());
  }*/
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
      if (event.type == sf::Event::MouseButtonPressed) {
	if (event.mouseButton.button == sf::Mouse::Left) {
	    hugoo.plant();
	    if(!hugoo.isCarrying())
	      for (unsigned int i =0; i<items.size();i++){
		if(items.at(i).getPosition()==hugoo.getPosition() && !items.at(i).isUsed())
		  hugoo.grab(&items.at(i));
	      }	
	    else hugoo.drop();
	}
      }
      if (event.type == sf::Event::KeyPressed) {
	switch (event.key.code) {
	  case sf::Keyboard::Escape: // Escape pressed : exit
	    window.close();
	    break;
	  case sf::Keyboard::D: // l pressed : Hugoo move right!
	    hugoo.moveRight();
	    break;
	  case sf::Keyboard::A: // h pressed : Hugoo move left! 					
	    hugoo.moveLeft();
	    break;
	  case sf::Keyboard::J: // j pressed : Hugoo stop moving! 					
	    hugoo.beIdle();
	    break;
	  case sf::Keyboard::S: // j pressed : Hugoo stop moving! 					
	    break;
	  case sf::Keyboard::E: // j pressed : Hugoo stop moving! 					
	    hugoo.plant();
	    if(!hugoo.hasPants())
	      for (unsigned int i =0; i<items.size();i++){
		if(items.at(i).getPosition()==hugoo.getPosition() && !items.at(i).isUsed())
		  hugoo.wear(&items.at(i));
	      }	
	    else hugoo.unwear();
	    /*
	       if(!hugoo.hasPants()){
	       if(hugoo.isCarrying()){
	       hugoo.plant();
	       hugoo.wear();
	       }
	       }
	       else {
	       if(!hugoo.isCarrying()){
	       hugoo.plant();
	       hugoo.unwear();
	       }
	       }
	       */
	    break;
	/*  case sf::Keyboard::F: // F pressed : Hugoo do something!
	    hugoo.plant();
	    if(!hugoo.isCarrying())
	      for (unsigned int i =0; i<items.size();i++){
		if(items.at(i).getPosition()==hugoo.getPosition() && !items.at(i).isUsed())
		  hugoo.grab(&items.at(i));
	      }	
	    else hugoo.drop();
	    //trees.emplace_back(myTree(hugoo.getPosition().x,hugoo.getPosition().y,&treeData));					
	    break;*/
	  case sf::Keyboard::T: // T pressed : toggle following					
	    toggleViewFollowHugoo = !toggleViewFollowHugoo;
	    break;
	    /* case sf::Keyboard::W:
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
	       break;*/
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
	    if(frameratelimit == 50) frameratelimit=1;
	    else frameratelimit=50;
	    window.setFramerateLimit(frameratelimit);
	    break;
	  default:					
	    break;
	}
      } else if(event.type == sf::Event::KeyReleased) {
	std::cout << "keycode: " << event.key.code << std::endl;
      }
    }
    hugoo.update();
    window.clear(bgColor);
    if(toggleViewFollowHugoo){view.setCenter(hugoo.getPosition().x,hugoo.getPosition().y-32);}
    window.setView(view);

    for (unsigned int i =0; i<trees.size();i++){
      trees.at(i).update();
      trees.at(i).draw(&window);
    }	
    //hugoo.draw(&window);

    for (unsigned int i =0; i<items.size();i++){
      items.at(i).update();
    //  items.at(i).draw(&window);
    }	

    window.draw(map);

    spriteManager.draw(&window);

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
