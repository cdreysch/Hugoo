#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include "myGameObject.h"
#include "myTileMap.h"

#define mainpath_win "C:/Users/cdreysch/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_unix

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
    // Hugoo
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
    hugooSprite.addAvailableStatusPtr(&hugooStandingCenter);
    hugooSprite.addAvailableStatusPtr(&hugooMovingRight);
    hugooSprite.addAvailableStatusPtr(&hugooMovingLeft);
    hugooSprite.addAvailableStatusPtr(&hugooDiggingBelowCenter);
    hugooSprite.setInterruptTable(hugooInterruptTable);
    hugooSprite.setIdleStatusNumber(0);
    hugooSprite.setIdle();

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
			break;
		    case sf::Keyboard::H: // h pressed : Hugoo move left! 
			hugooSprite.setStatus(2);
			break;
		    case sf::Keyboard::J: // j pressed : Hugoo stop moving! 
			//hugooSprite.setStatus(0);
			break;
		    case sf::Keyboard::F: // F pressed : Hugoo do something!
			hugooSprite.setStatus(3);
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
	window.draw(hugooSprite);

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
