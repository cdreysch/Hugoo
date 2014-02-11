#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#define mainpath_win "C:/Users/cdreysch/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_win

class myGameObject :public sf::Sprite{
private:
	std::vector<sf::Int32>* framesPerRectPtr;
	std::vector<sf::IntRect>* textureRectsPtr;
	std::vector<sf::Vector2f>* movesPerRectPtr;
	int status; 
	int statusFrame; 
	int currentRectIndex;
public:

	void setTextureRects(std::vector<sf::IntRect>* ptr){
		textureRectsPtr = ptr;
	}	
	void setFramesPerRect(std::vector<sf::Int32>* ptr){
		framesPerRectPtr = ptr;
	}	
	void setMovesPerRect(std::vector<sf::Vector2f>* ptr){
		movesPerRectPtr = ptr;
	}
	void initialize(){
		status = 0;
		statusFrame = 0;
		currentRectIndex = 0;
	}
	void update() {	    

		switch(status){
		case 0:
			statusFrame++;
			if(statusFrame>=framesPerRectPtr->at(currentRectIndex)){
				currentRectIndex++;
				if(currentRectIndex>=6){
					currentRectIndex=0;
				}
				setTextureRect(textureRectsPtr->at(currentRectIndex));
				move(movesPerRectPtr->at(currentRectIndex));
				statusFrame=0;
			}
			break;
		default:
			break;
		}
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

	int gridsize_texture = 16;
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
	if (!map.load(std::string("src/textures/minecraft_tiles16.png").insert(0,mainpath), sf::Vector2u(16, 16), &world, world_width, world_height))
		return -1;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Hugoo
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	sf::Image hugooImage;	
	if (!hugooImage.loadFromFile(std::string("src/textures/hugoo.bmp").insert(0,mainpath))){
		// error...
	}	
	hugooImage.createMaskFromColor(alphaColor);	

	std::vector<sf::IntRect> trStandingCenter;
	trStandingCenter.emplace_back(sf::IntRect(0*gridsize_texture,0*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trStandingCenter.emplace_back(sf::IntRect(1*gridsize_texture,0*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trStandingCenter.emplace_back(sf::IntRect(2*gridsize_texture,0*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trStandingCenter.emplace_back(sf::IntRect(3*gridsize_texture,0*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trStandingCenter.emplace_back(sf::IntRect(4*gridsize_texture,0*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trStandingCenter.emplace_back(sf::IntRect(5*gridsize_texture,0*gridsize_texture,gridsize_texture,3*gridsize_texture));

	std::vector<sf::Int32> fprStandingCenter;
	fprStandingCenter.emplace_back(100);
	fprStandingCenter.emplace_back(100);
	fprStandingCenter.emplace_back(100);
	fprStandingCenter.emplace_back(100);
	fprStandingCenter.emplace_back(100);
	fprStandingCenter.emplace_back(100);

	std::vector<sf::Vector2f> mprStandingCenter;
	mprStandingCenter.emplace_back(sf::Vector2f(0.f,0.f));
	mprStandingCenter.emplace_back(sf::Vector2f(0.f,0.f));
	mprStandingCenter.emplace_back(sf::Vector2f(0.f,0.f));
	mprStandingCenter.emplace_back(sf::Vector2f(0.f,0.f));
	mprStandingCenter.emplace_back(sf::Vector2f(0.f,0.f));
	mprStandingCenter.emplace_back(sf::Vector2f(0.f,0.f));

	std::vector<sf::IntRect> trMovingRight;
	trMovingRight.emplace_back(sf::IntRect(0*gridsize_texture,3*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trMovingRight.emplace_back(sf::IntRect(1*gridsize_texture,3*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trMovingRight.emplace_back(sf::IntRect(2*gridsize_texture,3*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trMovingRight.emplace_back(sf::IntRect(3*gridsize_texture,3*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trMovingRight.emplace_back(sf::IntRect(4*gridsize_texture,3*gridsize_texture,gridsize_texture,3*gridsize_texture));
	trMovingRight.emplace_back(sf::IntRect(5*gridsize_texture,3*gridsize_texture,gridsize_texture,3*gridsize_texture));

	std::vector<sf::Int32> fprMovingRight;
	fprMovingRight.emplace_back(3);
	fprMovingRight.emplace_back(3);
	fprMovingRight.emplace_back(3);
	fprMovingRight.emplace_back(3);
	fprMovingRight.emplace_back(3);
	fprMovingRight.emplace_back(3);

	std::vector<sf::Vector2f> mprMovingRight;
	mprMovingRight.emplace_back(sf::Vector2f(0.f,0.f));
	mprMovingRight.emplace_back(sf::Vector2f(0.f,0.f));
	mprMovingRight.emplace_back(sf::Vector2f(0.f,0.f));
	mprMovingRight.emplace_back(sf::Vector2f(2.f,0.f));
	mprMovingRight.emplace_back(sf::Vector2f(2.f,0.f));
	mprMovingRight.emplace_back(sf::Vector2f(2.f,0.f));

	std::vector<sf::IntRect> trMovingLeft;
	trMovingLeft.emplace_back(sf::IntRect(1*gridsize_texture,3*gridsize_texture,-gridsize_texture,3*gridsize_texture));
	trMovingLeft.emplace_back(sf::IntRect(2*gridsize_texture,3*gridsize_texture,-gridsize_texture,3*gridsize_texture));
	trMovingLeft.emplace_back(sf::IntRect(3*gridsize_texture,3*gridsize_texture,-gridsize_texture,3*gridsize_texture));
	trMovingLeft.emplace_back(sf::IntRect(4*gridsize_texture,3*gridsize_texture,-gridsize_texture,3*gridsize_texture));
	trMovingLeft.emplace_back(sf::IntRect(5*gridsize_texture,3*gridsize_texture,-gridsize_texture,3*gridsize_texture));
	trMovingLeft.emplace_back(sf::IntRect(6*gridsize_texture,3*gridsize_texture,-gridsize_texture,3*gridsize_texture));

	std::vector<sf::Int32> fprMovingLeft;
	fprMovingLeft.emplace_back(3);
	fprMovingLeft.emplace_back(3);
	fprMovingLeft.emplace_back(3);
	fprMovingLeft.emplace_back(3);
	fprMovingLeft.emplace_back(3);
	fprMovingLeft.emplace_back(3);

	std::vector<sf::Vector2f> mprMovingLeft;
	mprMovingLeft.emplace_back(sf::Vector2f(0.f,0.f));
	mprMovingLeft.emplace_back(sf::Vector2f(0.f,0.f));
	mprMovingLeft.emplace_back(sf::Vector2f(0.f,0.f));
	mprMovingLeft.emplace_back(sf::Vector2f(-2.f,0.f));
	mprMovingLeft.emplace_back(sf::Vector2f(-2.f,0.f));
	mprMovingLeft.emplace_back(sf::Vector2f(-2.f,0.f));

	sf::Texture hugooTexture;
	if (!hugooTexture.loadFromImage(hugooImage)){
		// error...
	}		

	myGameObject hugooSprite;
	hugooSprite.initialize();
	hugooSprite.setTexture(hugooTexture);
	hugooSprite.setTextureRect(trStandingCenter[3]);
	hugooSprite.setPosition(sf::Vector2f(20*gridsize_texture, 12*gridsize_texture));
	hugooSprite.setTextureRects(&trStandingCenter);
	hugooSprite.setFramesPerRect(&fprStandingCenter);
	hugooSprite.setMovesPerRect(&mprStandingCenter);	

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
					hugooSprite.setTextureRects(&trMovingRight);
					hugooSprite.setFramesPerRect(&fprMovingRight);
					hugooSprite.setMovesPerRect(&mprMovingRight);
					break;
				case sf::Keyboard::H: // h pressed : Hugoo move left! 
					hugooSprite.setTextureRects(&trMovingLeft);
					hugooSprite.setFramesPerRect(&fprMovingLeft);
					hugooSprite.setMovesPerRect(&mprMovingLeft);
					break;
				case sf::Keyboard::J: // j pressed : Hugoo stop moving! 
					hugooSprite.setTextureRects(&trStandingCenter);
					hugooSprite.setFramesPerRect(&fprStandingCenter);
					hugooSprite.setMovesPerRect(&mprStandingCenter);
					break;
				case sf::Keyboard::F: // F pressed : Hugoo do something!					
					std::cout << map.changeTile(hugooSprite.getPosition(),12) <<std::endl;
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
				std::cout << "isFullscreen: " << isFullscreen << std::endl;
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
