#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#define win32path "C:/Users/cdreysch/Hugoo/"

sf::String float2String(float floatvalue) {
	std::stringstream ss;	
	ss << floatvalue;
	return ss.str();
}
sf::String int642String(sf::Int64 integer) {
	std::stringstream ss;	
	ss << integer;
	return ss.str();
}

int main()
{
	// Display the list of all the video modes available for fullscreen
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	for (std::size_t i = 0; i < modes.size(); ++i) {
		sf::VideoMode mode = modes[i];
		std::cout << "Mode #" << i << ": " << mode.width << "x" << mode.height << " - " 
			<< mode.bitsPerPixel << " bpp" << std::endl;
	}

	sf::Font font;
	if (!font.loadFromFile("src/fonts/DejaVuSansMono-Bold.ttf")){
		// error...
	}
	sf::Text text;
	text.setFont(font); // font is a sf::Font
	text.setCharacterSize(16); // in pixels, not points!
	text.setColor(sf::Color(50,50,50));
	//text.setStyle(sf::Text::Bold | sf::Text::Underlined);

	sf::Int64 totalFrame = 0;

	int width = 640;
	int height = 480;	
	int blocksize = 16;

	sf::Color bgColor = sf::Color(50,50,50);
	sf::Color alphaColor = sf::Color(0,0,0);

	sf::VideoMode fullscreenMode = modes[3];
	sf::VideoMode windowedMode = sf::VideoMode(width, height);
	sf::RenderWindow window(windowedMode, "Hugoo");	
	bool isFullscreen = false;
	int frameratelimit = 30;
	window.setFramerateLimit(frameratelimit);


	sf::Image tileset;
	if (!tileset.loadFromFile("src/textures/tileset.bmp")){
		// error...
	}
	// interpretiere tileset ==> XML?
	sf::IntRect stone = sf::IntRect(0,0,blocksize,blocksize);
	sf::IntRect air = sf::IntRect(blocksize,0,blocksize,blocksize);
	sf::IntRect mud = sf::IntRect(blocksize,blocksize,blocksize,blocksize);
	sf::IntRect grass = sf::IntRect(0,blocksize,blocksize,blocksize);
	
	sf::Image hugooImage;
	if (!hugooImage.loadFromFile("src/textures/hugoo.bmp")){
		// error...
	}	
	hugooImage.createMaskFromColor(alphaColor);

	// interpretiere hugooImage ==> XML?
	sf::IntRect eyes4 = sf::IntRect(0,0,blocksize,3*blocksize);
	sf::IntRect eyes5 = sf::IntRect(blocksize,0,blocksize,3*blocksize);
	sf::IntRect eyes6 = sf::IntRect(2*blocksize,0,blocksize,3*blocksize);
	sf::IntRect eyes2 = sf::IntRect(3*blocksize,0,blocksize,3*blocksize);
	sf::IntRect eyes8 = sf::IntRect(4*blocksize,0,blocksize,3*blocksize);
	sf::IntRect eyes9 = sf::IntRect(5*blocksize,0,blocksize,3*blocksize);


	sf::Image screenImage;
	screenImage.create(width,height,bgColor);

	// generate World
	int iy;
	int ix;
	for (iy=0;iy<height/blocksize;iy++){
		for (ix=0;ix<width/blocksize;ix++){
			sf::IntRect medium = air;			
			if(iy==15){
				medium = grass;
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

	sf::Sprite hugooSprite;
	hugooSprite.setTexture(hugooTexture);
	hugooSprite.setTextureRect(eyes4);
	hugooSprite.setPosition(sf::Vector2f(20*blocksize, 13*blocksize));
	
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
		window.draw(hugooSprite);

		totalFrame++;
		if (totalFrame==10){
			sf::Time elapsedTime = clock.restart();
			text.setString(float2String(10.f/elapsedTime.asSeconds()));
			totalFrame = 0;
		}
		window.draw(text);

		window.display();
	}
	return 0;
}
