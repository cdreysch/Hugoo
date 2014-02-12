#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#define mainpath_win "C:/Users/cdreysch/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_win

struct actionTrigger {int triggerFrame; sf::IntRect textureRect; sf::Vector2f moveVector;};

class myObjectStatus {
private:
	std::vector<actionTrigger> actionTriggerStack;
	int currentTriggerIndex;
	int totalStatusFrames;
	int currentStatusFrame;		

public:
	void addActionTrigger(int trigFrame, sf::IntRect textRect, sf::Vector2f moveVec){
		actionTrigger at = { trigFrame, textRect, moveVec };
		actionTriggerStack.emplace_back(at);
	}
	actionTrigger getCurrentActionTrigger() {
		return actionTriggerStack.at(currentTriggerIndex);
	}
	actionTrigger getNextActionTrigger() {
		actionTrigger nextAT;
		if(currentTriggerIndex<actionTriggerStack.size()-1 && currentTriggerIndex!=0)
			nextAT = actionTriggerStack.at(currentTriggerIndex+1);
		else 			
			nextAT = actionTriggerStack.at(0);
		return nextAT;
	}
	bool hasEnded(){		
		return (currentStatusFrame>=totalStatusFrames);
	}
	void restart(){
		currentTriggerIndex = 0;
		currentStatusFrame = 0;
	}

	int tick(){
		if(!hasEnded()) {
			currentStatusFrame++;				
			if(currentStatusFrame==getNextActionTrigger().triggerFrame){			
				currentTriggerIndex++;				
				return 1;		
			}
		}
		return 0;
	}	
	myObjectStatus(sf::IntRect textureRect,int totalFrames){
		totalStatusFrames = totalFrames;		
		currentStatusFrame = 0;
		currentTriggerIndex = 0;
		addActionTrigger(1, textureRect, sf::Vector2f(0.f,0.f));		
	}
};

class myGameObject :public sf::Sprite{
private:	
	std::vector<myObjectStatus*> availableStatusPtr;
	std::vector<int> statusNumberStack;
	int currentStackIndex;	
public:	
	void addAvailableStatusPtr(myObjectStatus* statusPtr){
		availableStatusPtr.push_back(statusPtr);		
	}	
	void addStatusToStack(int number){		
		statusNumberStack.push_back(number);		
	}
	myObjectStatus* getCurrentStatusPtr(){		
		return availableStatusPtr.at(statusNumberStack.at(currentStackIndex));
	}
	void nextStatus(){
		if(isIdle() || getCurrentStatusPtr()->hasEnded()){			
			if(statusNumberStack.size()>2){
				statusNumberStack.erase(statusNumberStack.begin()+1);
				currentStackIndex = 1;
				getCurrentStatusPtr()->restart();
			}else if(statusNumberStack.size()==2){				
				currentStackIndex = 1;
				getCurrentStatusPtr()->restart();
			}else 
				setIdle();
		}
	}
	void setStatus(int number){
		addStatusToStack(number);		
	}
	bool isIdle(){
		return currentStackIndex == 0;
	}
	void setIdle(){
		currentStackIndex = 0;
		getCurrentStatusPtr()->restart();
	}
	void update() {
		if(isIdle() && statusNumberStack.size()>1){
			currentStackIndex == 1;
		}
		if(getCurrentStatusPtr()->tick()){			
			setTextureRect(getCurrentStatusPtr()->getCurrentActionTrigger().textureRect);			
			move(getCurrentStatusPtr()->getCurrentActionTrigger().moveVector);				
		}
		if(getCurrentStatusPtr()->hasEnded())
			nextStatus();
	}
};

class myTileMap : public sf::Drawable, public sf::Transformable{
public:
	bool load(const std::string& tileset, sf::Vector2u tileSize, std::vector<int>* tiles, unsigned int width, unsigned int height)    {
		// load the tileset image
		sf::Image tilesetImage;
		if (!tilesetImage.loadFromFile(tileset))
			return false;		
		tilesetImage.createMaskFromColor(tilesetImage.getPixel(0,0));

		// load the tileset texture
		if (!m_tileset.loadFromImage(tilesetImage))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		// populate the vertex array, with one quad per tile
		for (unsigned int i = 0; i < width; ++i)
			for (unsigned int j = 0; j < height; ++j){

				// get the current tile number
				int tileNumber = tiles->at(i + j * width);

				// find its position in the tileset texture
				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}

			m_tilesize = tileSize;
			m_width = width;
			m_height = height;
			return true;
	}

	int changeTile(sf::Vector2f pos, unsigned int tileNumber)    {
		int x = (int)pos.x/m_tilesize.x;
		int y = ((int)pos.y/m_tilesize.y)+2;

		// find its position in the tileset texture
		int tu = tileNumber % (m_tileset.getSize().x / m_tilesize.x);
		int tv = tileNumber / (m_tileset.getSize().x / m_tilesize.x);

		// get a pointer to the tile's quad
		sf::Vertex* quad = &m_vertices[(x + y * m_width) * 4];

		// define its 4 texture coordinates
		quad[0].texCoords = sf::Vector2f(tu * m_tilesize.x, tv * m_tilesize.y);
		quad[1].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, tv * m_tilesize.y);
		quad[2].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, (tv + 1) * m_tilesize.y);
		quad[3].texCoords = sf::Vector2f(tu * m_tilesize.x, (tv + 1) * m_tilesize.y);
		return x;
	}

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const    {

		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
	sf::Vector2u m_tilesize;
	unsigned int m_width; 
	unsigned int m_height;
};

int main()
{
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

	sf::Image hugooImage;	
	if (!hugooImage.loadFromFile(std::string("src/textures/hugoo.bmp").insert(0,mainpath))){
		// error...
	}	
	hugooImage.createMaskFromColor(alphaColor);	

	/////////////////////////////// STATUS: Standing Center /////////////////////////////////////////////
	myObjectStatus hugooStandingCenter = myObjectStatus(sf::IntRect(0*tilesize,0*tilesize,tilesize,3*tilesize),600);
	hugooStandingCenter.addActionTrigger(100, sf::IntRect(1*tilesize,0*tilesize,tilesize,3*tilesize), sf::Vector2f(0.f,0.f));
	hugooStandingCenter.addActionTrigger(200, sf::IntRect(2*tilesize,0*tilesize,tilesize,3*tilesize), sf::Vector2f(0.f,0.f));
	hugooStandingCenter.addActionTrigger(300, sf::IntRect(3*tilesize,0*tilesize,tilesize,3*tilesize), sf::Vector2f(0.f,0.f));
	hugooStandingCenter.addActionTrigger(400, sf::IntRect(4*tilesize,0*tilesize,tilesize,3*tilesize), sf::Vector2f(0.f,0.f));
	hugooStandingCenter.addActionTrigger(500, sf::IntRect(5*tilesize,0*tilesize,tilesize,3*tilesize), sf::Vector2f(0.f,0.f));

	/////////////////////////////// STATUS: Moving Right ////////////////////////////////////////////
	myObjectStatus hugooMovingRight = myObjectStatus(sf::IntRect(0*tilesize,3*tilesize,tilesize,3*tilesize),33);
	hugooMovingRight.addActionTrigger(3, sf::IntRect(1*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(1.f,0.f));
	hugooMovingRight.addActionTrigger(6, sf::IntRect(2*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(1.f,0.f));
	hugooMovingRight.addActionTrigger(9, sf::IntRect(3*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(2.f,0.f));
	hugooMovingRight.addActionTrigger(12, sf::IntRect(4*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(2.f,0.f));
	hugooMovingRight.addActionTrigger(15, sf::IntRect(5*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(2.f,0.f));
	hugooMovingRight.addActionTrigger(18, sf::IntRect(1*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(1.f,0.f));
	hugooMovingRight.addActionTrigger(21, sf::IntRect(2*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(1.f,0.f));
	hugooMovingRight.addActionTrigger(24, sf::IntRect(3*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(2.f,0.f));
	hugooMovingRight.addActionTrigger(27, sf::IntRect(4*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(2.f,0.f));
	hugooMovingRight.addActionTrigger(30, sf::IntRect(5*tilesize,3*tilesize,tilesize,3*tilesize), sf::Vector2f(2.f,0.f));

	/////////////////////////////// STATUS: Moving Left ////////////////////////////////////////////
	myObjectStatus hugooMovingLeft = myObjectStatus(sf::IntRect(1*tilesize,3*tilesize,-tilesize,3*tilesize),33);
	hugooMovingLeft.addActionTrigger(3, sf::IntRect(2*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-1.f,0.f));
	hugooMovingLeft.addActionTrigger(6, sf::IntRect(3*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-1.f,0.f));
	hugooMovingLeft.addActionTrigger(9, sf::IntRect(4*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-2.f,0.f));
	hugooMovingLeft.addActionTrigger(12, sf::IntRect(5*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-2.f,0.f));
	hugooMovingLeft.addActionTrigger(15, sf::IntRect(6*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-2.f,0.f));
	hugooMovingLeft.addActionTrigger(18, sf::IntRect(2*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-1.f,0.f));
	hugooMovingLeft.addActionTrigger(21, sf::IntRect(3*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-1.f,0.f));
	hugooMovingLeft.addActionTrigger(24, sf::IntRect(4*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-2.f,0.f));
	hugooMovingLeft.addActionTrigger(27, sf::IntRect(5*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-2.f,0.f));
	hugooMovingLeft.addActionTrigger(30, sf::IntRect(6*tilesize,3*tilesize,-tilesize,3*tilesize), sf::Vector2f(-2.f,0.f));

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
	hugooSprite.addStatusToStack(0);
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
					map.changeTile(hugooSprite.getPosition(),12);
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
