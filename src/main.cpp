#include <SFML/Graphics.hpp>

int main()
{
	int width = 640;
	int height = 400;	
	int block_size = 16;
	sf::Color bgColor = sf::Color(50,50,50);
	sf::Color alphaColor = sf::Color(0,0,0);


	sf::RenderWindow window(sf::VideoMode(width, height), "Hugoo");	

	sf::Image tileset;
	if (!tileset.loadFromFile("C:/Users/cdreysch/Hugoo/src/textures/tileset.bmp"))
	{
		// error...
	}
	// interpretiere tileset ==> XML?
	sf::IntRect stone = sf::IntRect(0,0,block_size,block_size);
	sf::IntRect air = sf::IntRect(block_size,0,block_size,block_size);
	sf::IntRect mud = sf::IntRect(block_size,block_size,block_size,block_size);
	sf::IntRect grass = sf::IntRect(0,block_size,block_size,block_size);
	
	sf::Image hugooImage;
	if (!hugooImage.loadFromFile("C:/Users/cdreysch/Hugoo/src/textures/hugoo.bmp"))
	{
		// error...
	}	
	hugooImage.createMaskFromColor(alphaColor);

	// interpretiere hugooImage ==> XML?
	sf::IntRect eyes4 = sf::IntRect(0,0,block_size,3*block_size);
	sf::IntRect eyes5 = sf::IntRect(block_size,0,block_size,3*block_size);
	sf::IntRect eyes6 = sf::IntRect(2*block_size,0,block_size,3*block_size);
	sf::IntRect eyes2 = sf::IntRect(3*block_size,0,block_size,3*block_size);
	sf::IntRect eyes8 = sf::IntRect(4*block_size,0,block_size,3*block_size);
	sf::IntRect eyes9 = sf::IntRect(5*block_size,0,block_size,3*block_size);


	sf::Image screenImage;
	screenImage.create(width,height,bgColor);

	// generate World
	int iy;
	int ix;
	for (iy=0;iy<25;iy++)
	{
		for (ix=0;ix<40;ix++)
		{
			sf::IntRect medium = air;			
			if(iy==15)
			{
				medium = grass;
			}
			if(iy>15)
			{
				medium = stone;
			}
			screenImage.copy(tileset,ix*block_size,iy*block_size,medium,false);
			
		}
	}	

	sf::Texture screenTexture;
	if (!screenTexture.loadFromImage(screenImage))
	{
		// error...
	}

	sf::Texture hugooTexture;
	if (!hugooTexture.loadFromImage(hugooImage))
	{
		// error...
	}

	sf::Sprite screenSprite;
	screenSprite.setTexture(screenTexture);

	sf::Sprite hugooSprite;
	hugooSprite.setTexture(hugooTexture);
	hugooSprite.setTextureRect(eyes4);
	hugooSprite.setPosition(sf::Vector2f(20*block_size, 13*block_size));
	


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(screenSprite);
		window.draw(hugooSprite);
		window.display();
	}

	return 0;
}