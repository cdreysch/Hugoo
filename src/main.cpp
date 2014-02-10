#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#define win32path "C:/Users/cdreysch/Hugoo/"
#define unixpath ""
#define exepath win32path

class myGameObject :public sf::Sprite{
private:
	std::vector<sf::Int32> framesPerRectPtr;
	std::vector<sf::IntRect> textureRectsPtr;
	std::vector<sf::Vector2f> movesPerRectPtr;
	int status; 
	int statusFrame; 
	int currentRectIndex;
public:

	void setTextureRects(std::vector<sf::IntRect> ptr){
		textureRectsPtr = ptr;
	}	
	void setFramesPerRect(std::vector<sf::Int32> ptr){
		framesPerRectPtr = ptr;
	}	
	void setMovesPerRect(std::vector<sf::Vector2f> ptr){
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
			if(statusFrame>=framesPerRectPtr[currentRectIndex]){
				currentRectIndex++;
				if(currentRectIndex>=6){
					currentRectIndex=0;
				}
				setTextureRect(textureRectsPtr[currentRectIndex]);
				move(movesPerRectPtr[currentRectIndex]);
				statusFrame=0;
			}
			break;
		default:
			break;
		}
	}
};

int main()
{
	int width = 640;
	int height = 480;	
	int blocksize = 16;
	
	// Display the list of all the video modes available for fullscreen
	int fullscreenModeIndex=0;
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	for (std::size_t i = 0; i < modes.size(); ++i) {
		sf::VideoMode mode = modes[i];
		std::cout << "Mode #" << i << ": " << mode.width << "x" << mode.height << " - " 
			<< mode.bitsPerPixel << " bpp";
		if((float)mode.width/(float)mode.height==16.f/9.f){			
			std::cout << "  ,16:9";
		}
		if(mode.width==width&&mode.height==height&&mode.bitsPerPixel==32){
			fullscreenModeIndex = i;
			std::cout << "  <-- selected";
		}
		std::cout << std::endl;
	}

	sf::Font font;
	if (!font.loadFromFile(std::string("src/fonts/FreeMono.ttf").insert(0,exepath))){
		// error...
	}
	sf::Text text;
	text.setFont(font); // font is a sf::Font
	text.setCharacterSize(12); // in pixels, not points!
	text.setColor(sf::Color(50,50,50));

	sf::Int64 totalFrames = 0;
	sf::Int32 elapsedFrames = 0;
	sf::Int32 fpsUpdateFrequency = 10; //Hertz
	sf::Int32 frameratelimit = 50;

	sf::Color bgColor = sf::Color(50,50,50);
	sf::Color alphaColor = sf::Color(0,0,0);

	sf::VideoMode fullscreenMode = modes[fullscreenModeIndex];
	sf::VideoMode windowedMode = sf::VideoMode(width, height);
	sf::RenderWindow window(windowedMode, "Hugoo");	
	bool isFullscreen = false;
	window.setFramerateLimit(frameratelimit);

	sf::Image tileset;
	if (!tileset.loadFromFile(std::string("src/textures/tileset.bmp").insert(0,exepath))){
		// error...
	}
	// interpretiere tileset ==> XML?
	sf::IntRect stone = sf::IntRect(0,0,blocksize,blocksize);
	sf::IntRect air = sf::IntRect(blocksize,0,blocksize,blocksize);
	sf::IntRect mud = sf::IntRect(blocksize,blocksize,blocksize,blocksize);
	sf::IntRect grass = sf::IntRect(0,blocksize,blocksize,blocksize);

	sf::Image hugooImage;
	if (!hugooImage.loadFromFile(std::string("src/textures/hugoo.bmp").insert(0,exepath))){
		// error...
	}	
	hugooImage.createMaskFromColor(alphaColor);

	// interpretiere hugooImage ==> XML?
	std::vector<sf::IntRect> trStandingCenter;
	trStandingCenter.emplace_back(sf::IntRect(0,0*blocksize,blocksize,3*blocksize));
	trStandingCenter.emplace_back(sf::IntRect(blocksize,0*blocksize,blocksize,3*blocksize));
	trStandingCenter.emplace_back(sf::IntRect(2*blocksize,0*blocksize,blocksize,3*blocksize));
	trStandingCenter.emplace_back(sf::IntRect(3*blocksize,0*blocksize,blocksize,3*blocksize));
	trStandingCenter.emplace_back(sf::IntRect(4*blocksize,0*blocksize,blocksize,3*blocksize));
	trStandingCenter.emplace_back(sf::IntRect(5*blocksize,0*blocksize,blocksize,3*blocksize));

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
	trMovingRight.emplace_back(sf::IntRect(0,3*blocksize,blocksize,3*blocksize));
	trMovingRight.emplace_back(sf::IntRect(blocksize,3*blocksize,blocksize,3*blocksize));
	trMovingRight.emplace_back(sf::IntRect(2*blocksize,3*blocksize,blocksize,3*blocksize));
	trMovingRight.emplace_back(sf::IntRect(3*blocksize,3*blocksize,blocksize,3*blocksize));
	trMovingRight.emplace_back(sf::IntRect(4*blocksize,3*blocksize,blocksize,3*blocksize));
	trMovingRight.emplace_back(sf::IntRect(5*blocksize,3*blocksize,blocksize,3*blocksize));

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
	
	sf::Image screenImage;
	screenImage.create(width,height,bgColor);

	// generate World
	int iy;
	int ix;

	for (iy=0;iy<height/blocksize;iy++){
		for (ix=0;ix<width/blocksize;ix++){
			sf::IntRect medium = air;			
			if(iy==15){
				medium = mud;
			}
			if(iy>15){
				medium = stone;
			}
			screenImage.copy(tileset,ix*blocksize,iy*blocksize,medium,false);
		}
	}	

	sf::Texture screenTexture;
	if (!screenTexture.loadFromImage(screenImage)){
		// error...
	}

	sf::Texture hugooTexture;
	if (!hugooTexture.loadFromImage(hugooImage)){
		// error...
	}

	sf::Sprite screenSprite;
	screenSprite.setTexture(screenTexture);

	myGameObject hugooSprite;
	hugooSprite.initialize();
	hugooSprite.setTexture(hugooTexture);
	hugooSprite.setTextureRect(trStandingCenter[3]);
	hugooSprite.setPosition(sf::Vector2f(20*blocksize, 12*blocksize));
	hugooSprite.setTextureRects(trStandingCenter);
	hugooSprite.setFramesPerRect(fprStandingCenter);
	hugooSprite.setMovesPerRect(mprStandingCenter);

	sf::Clock clock;
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
				case sf::Keyboard::L: // l pressed : move right 
					hugooSprite.setTextureRects(trMovingRight);
					hugooSprite.setFramesPerRect(fprMovingRight);
					hugooSprite.setMovesPerRect(mprMovingRight);
					break;
				case sf::Keyboard::J: // j pressed : stand center 
					hugooSprite.setTextureRects(trStandingCenter);
					hugooSprite.setFramesPerRect(fprStandingCenter);
					hugooSprite.setMovesPerRect(mprStandingCenter);
					break;
				case sf::Keyboard::F11: // F11 pressed: fullscreen toggle
					if (!isFullscreen) {
						window.close();
						window.create(fullscreenMode,"Hugoo",sf::Style::Fullscreen);
						window.setFramerateLimit(frameratelimit);
						isFullscreen = true;
						//screenSprite.setScale(sf::Vector2f(2.f,2.f));
						//hugooSprite.setScale(sf::Vector2f(2.f,2.f));
					}else {
						window.close();
						window.create(windowedMode,"Hugoo");
						window.setFramerateLimit(frameratelimit);
						isFullscreen = false;
					}
					break;
				default:
					//game->handleKeyCode(event.key.code);
					break;
				}
			} else if(event.type == sf::Event::KeyReleased) {
				std::cout << "isFullscreen: " << isFullscreen << std::endl;
			}
		}
		window.clear();
		window.draw(screenSprite);
		hugooSprite.update();
		window.draw(hugooSprite);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// this code should remind me to implement a VERTEX based tile engine
		sf::Color testColor = sf::Color(220,150,150);
		sf::Vertex vertices[] =
		{
			sf::Vertex(sf::Vector2f(  0,   0), testColor, sf::Vector2f( 0,  0)),
			sf::Vertex(sf::Vector2f(  0, 6*blocksize), testColor, sf::Vector2f( 0, 3*blocksize)),
			sf::Vertex(sf::Vector2f(2*blocksize, 6*blocksize), testColor, sf::Vector2f(blocksize, 3*blocksize)),
			sf::Vertex(sf::Vector2f(2*blocksize,   0), testColor, sf::Vector2f(blocksize,  0))
		};
		// draw it
		window.draw(vertices, 4, sf::Quads,sf::RenderStates(&hugooTexture));
		///////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		window.draw(text);

		window.display();
	}
	return 0;
}
