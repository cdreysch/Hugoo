#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include "myGameObject.h"
#include "myTileMap.h"
#include "myAnimatedObject.h"

#define mainpath_win "C:/Users/cdreysch/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_unix

class myTree :public myGameObject {
  public:
    myTree(int x, int y, sf::Texture* textur, std::vector<myObjectStatus> aos, std::vector<bool> interruptTable) {		
      setPosition(sf::Vector2f(x, y)); 
      setTexture(*textur);
      setAvailableStatus(aos);
      setInterruptTable(interruptTable);
      setIdleStatusNumber(0);
      setIdle();
      setStatus(1);
    }
};

class myHuman {
  public:
    myHuman(int x,int y, myAnimationData* bodyDataPtr, myAnimationData* hoseDataPtr){
      body = myAnimatedObject(x,y,bodyDataPtr);
      hose = myAnimatedObject(x,y,hoseDataPtr);
    }
    void setInterruptionTable(bool* bits[]){

    }
    void draw(sf::RenderWindow *target){
      target->draw(body);
      target->draw(hose);
    }
    void update(){
      body.update();
      hose.update();
    }
    void moveRight(){
      body.changeSequence(2);
      hose.changeSequence(2);
    }
    void beIdle(){
      body.changeSequence(0);
      hose.changeSequence(0);
    }
    void plant(){
      body.changeSequence(1);
      hose.changeSequence(1);
    }

  private:
    std::vector<bool> interruptionTable;
    myAnimatedObject body;
    myAnimatedObject hose;
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

  /* GameObjects sollten später durch eine Methode von GameObject aus einer Konfigurationsdatei 
   * (pro Object) eingelesen werden. Auch der Name der Textur könnte dort hinterlegt sein.
   * Wie es bei Erben von GameObject sein soll, weiß ich noch nicht...
   * Eventuell muss die entsprechende Methode überschrieben werden, um zusätzliche Attribute einzulesen.
   * 
   * Am Ende sollten die benötigten GameObjects direkt aus der Weltbeschreibung generiert werden.
   * (zB. liegt ein "Hugoo" auf irgendeiner Koordiante -> try myGameObject('Hugoo.cfg',...))
   * Die Beschreibung der Objekte innerhalb der Welt sollte seperat zur Geographie abgespeichert werden.
   */

  ////////////////////////////// Hugoo //////////////////////////////////////////////////////////
  sf::Image hugooImage;	
  if (!hugooImage.loadFromFile(std::string("src/textures/hugoo.png").insert(0,mainpath))){
    // error...
  }	
  hugooImage.createMaskFromColor(alphaColor);	

#include "hugooStatus.cfg"

  const int bits[] = {1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,0,0};
  std::vector<bool> hugooInterruptTable (bits, bits + sizeof(bits) / sizeof(bits[0]));	

  sf::Texture hugooTexture;
  if (!hugooTexture.loadFromImage(hugooImage)){
    // error...
  }		

  myGameObject hugooSprite;
  hugooSprite.setTexture(hugooTexture);	
  hugooSprite.setPosition(sf::Vector2f(20*tilesize, 12*tilesize));
  hugooSprite.addAvailableStatus(hugooStandingCenter);
  hugooSprite.addAvailableStatus(hugooMovingRight);
  hugooSprite.addAvailableStatus(hugooMovingLeft);
  hugooSprite.addAvailableStatus(hugooDiggingBelowCenter);
  hugooSprite.setInterruptTable(hugooInterruptTable);
  hugooSprite.setIdleStatusNumber(0);
  hugooSprite.setIdle();

  ////////////////////////////// Tree //////////////////////////////////////////////////////////
  sf::Image treeImage;			
  if (!treeImage.loadFromFile(std::string("src/textures/tree.png").insert(0,mainpath))){
    // error...
  }	
  treeImage.createMaskFromColor(alphaColor);
  sf::Texture treeTexture;
  if (!treeTexture.loadFromImage(treeImage)){
    // error...
  }	
  sf::Texture treeTexture2;
  if (!treeTexture2.loadFromImage(treeImage)){
    // error...
  }

#include "treeStatus.cfg"
  std::vector<myObjectStatus> treeStatus;
  treeStatus.push_back(treeStanding);
  treeStatus.push_back(treeGrowing);

  const int tbits[] = {1,1, 1,1};
  std::vector<bool> treeInterruptTable (tbits, tbits + sizeof(tbits) / sizeof(tbits[0]));	    		

  myTree tree1 = myTree(25*tilesize,14*tilesize,&treeTexture,treeStatus,treeInterruptTable);
  myTree tree2 = myTree(15*tilesize,14*tilesize,&treeTexture2,treeStatus,treeInterruptTable);	

  /* PROBLEM: Es wird bei jeder Instanz von myTree der SELBE Status verändert. 
   * LÖSUNG: Es sollte eine Klasse Animation eingeführt werden, mit definierten Rechtecken in einer Reihenfolge und zugehörigen Verschiebungskoordinaten. 
   * Für jede Objektklasse müsste die Animationen nur als Zeiger übergeben werden.
   * Zusätzlich wird für die Klasse Status benutzt, um für jede Instanz der Objektklassen den derzeitigen Status zu verwalten.
   * ActionTrigger würden dann nur die Nummer des Rechtecks in der jeweiligen Animation angeben, nicht das Rechteck selbst.
   */

  mySequence seq0 = mySequence("Standing",600);
  seq0.addEntry(0,	0,0,1,3,	0,-2,	tilesize,	0,0);
  seq0.addEntry(100,	1,0,1,3,	0,-2,	tilesize,	0,0);
  seq0.addEntry(200,	2,0,1,3,	0,-2,	tilesize,	0,0);
  seq0.addEntry(300,	3,0,1,3,	0,-2,	tilesize,	0,0);
  seq0.addEntry(400,	4,0,1,3,	0,-2,	tilesize,	0,0);
  seq0.addEntry(500,	5,0,1,3,	0,-2,	tilesize,	0,0);

  mySequence seq1 = mySequence("Planting",30);
  seq1.addEntry(0,	3,0,1,3,	0,-2,	tilesize,	0,0);
  seq1.addEntry(10,	0,6,1,3,	0,-2,	tilesize,	0,0);
  seq1.addEntry(20,	1,6,1,3,	0,-2,	tilesize,	0,0);

  mySequence seq2 = mySequence("Moving Right",50);
  seq2.addEntry(0,	0,3,1,3,	0,-2,	tilesize,	0,0);
  seq2.addEntry(10,	1,3,1,3,	0,-2,	tilesize,	0,0);
  seq2.addEntry(20,	2,3,1,3,	0,-2,	tilesize,	2,0);
  seq2.addEntry(30,	3,3,1,3,	0,-2,	tilesize,	2,0);
  seq2.addEntry(40,	4,3,1,3,	0,-2,	tilesize,	2,0);

  myAnimationData hugooData = myAnimationData(std::string("src/textures/huNaked.png").insert(0,mainpath));
  hugooData.addTexture(std::string("src/textures/huHose1.png").insert(0,mainpath));
  hugooData.addSequence(seq0);
  hugooData.addSequence(seq1);
  hugooData.addSequence(seq2);

  myAnimationData huHoseData = myAnimationData(std::string("src/textures/huHose1.png").insert(0,mainpath));
  huHoseData.addSequence(seq0);
  huHoseData.addSequence(seq1);
  huHoseData.addSequence(seq2);

  myAnimatedObject hugoo2 = myAnimatedObject(16*tilesize,14*tilesize,&hugooData);
  //hugoo2.addQuad(16*tilesize,14*tilesize,&huHoseData);
  //hugoo2.setQuadColor(1,sf::Color(205,205,150,255));
  myHuman hugoo3 = myHuman(12*tilesize,14*tilesize,&hugooData,&huHoseData);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Game Loop
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  view.setCenter(hugooSprite.getPosition());
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
	    hugooSprite.setStatus(1);
	    hugoo2.changeSequence(2);
	    hugoo3.moveRight();
	    break;
	  case sf::Keyboard::H: // h pressed : Hugoo move left! 
	    hugooSprite.setStatus(2);
	    break;
	  case sf::Keyboard::J: // j pressed : Hugoo stop moving! 
	    hugoo2.changeSequence(0);
	    hugoo3.beIdle();
	    break;
	  case sf::Keyboard::F: // F pressed : Hugoo do something!
	    hugooSprite.setStatus(3);
	    hugoo2.changeSequence(1);
	    hugoo3.plant();
	    map.changeTile(hugooSprite.getPosition(),(rand() % 2) + 12 + (rand() % 2)*16);
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
    if(toggleViewFollowHugoo){view.setCenter(hugooSprite.getPosition());}
    window.setView(view);
    window.draw(map);
    hugooSprite.update();
    tree1.update();
    tree2.update();
    
    hugoo3.update();
    hugoo3.draw(&window);

    window.draw(tree1);
    window.draw(tree2);	

    window.draw(hugooSprite);

    hugoo2.update();
    window.draw(hugoo2);

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
