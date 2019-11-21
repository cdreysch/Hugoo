#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <libconfig.h++>
#include "../Canvas.h"
#include "../SrcImage.h"

#define mainpath_win "C:/Users/Chris/source/repos/Hugoo/"
#define mainpath_unix ""
#define mainpath mainpath_win

/*
TODO-LIST:
+ make editor more powerfull:
	- resizeable tiles: automatic rearrrangement, resizing, creation/deletion of all related images, sequences
	- save/create modified/new config files (needed for sequences and others)
	- center the origin of all canvases (otherwise we need the offsetXY for each tile to keep the figure in place when tile size changes)		
	- add gui to edit mode:
		* windows left and right in high resolution
		* textfields, buttons, dropdown menues, checkboxes...
		* select Canvas/Sequence from AnimationObject to edit
+ work on the game engine:
	- plan different object types (fix base classes for humans, wolfs, birds, items, houses, backgrounds, plants, container...)
	- introduce config files for custom variants of these classes (i.e. "Bandit-Boss": base_class "human", size "big", shape ""fat" gender "male", hair "bold", face "scars01", player-reknown "hostile",....
	- introduce config file for the world/objects (map, season, plants: ({class: "tree01", pos:...},{...}))
	- test performance/behavior with multiple instances of objects
	- triggerable effects (spawn new objects ,i.e. arrows, flasks)
	- hitboxes (most important: solids to walk on/ obstancle -> jump&run behavior, zones for each frame (new SrcImages!) for hit detection, mechanism to trigger effects when hit (reduce health, interrupt animation, ...)
	- "Souls" as bots for objects (with properties that transfer over)
*/

class Entry {
public:
	sf::IntRect tile;
	sf::Vector2f move_xy;
	int move_z;
	Entry(int tile_x = 0, int tile_y = 0, int tile_w = 0, int tile_h = 0, float move_x = 0, float move_y = 0, float move_z = 0) {
		tile = sf::IntRect(tile_x, tile_y, tile_w, tile_h);
		move_xy = sf::Vector2f(move_x, move_y);
		this->move_z = move_z;
	}
	sf::IntRect getTile() { return tile; }
};

class Sequence {
public:
	std::map<int, Entry> sequence;
	std::map<int, Entry>::iterator it;
	int length;

	Sequence() {
		length = 0;
	}
	void addEntry(int x, int y, int w, int h, int d) {
		std::cout << "    (" << x << "," << y << "," << w << "," << h << "," << d << ")" << std::endl;
		sequence[length] = Entry(x, y, w, h, 0, 0, 0); // TODO: replace 0s
		length += d;
	}
	bool hasEntryAt(int frame_number) {
		it = sequence.find(frame_number);
		if (it != sequence.end()) {
			return true;
		}
		else return false;
	}
	Entry getEntryAt(int frame_number) {
		if (hasEntryAt(frame_number)) {
			return sequence[frame_number];
		}
		else {
			std::cout << "Warning: Empty entry returned!" << std::endl;
			return Entry(0, 0, 0, 0, 0, 0, 0); //dummy entry
		}
	}
};

std::map<std::string, Sequence*> global_sequences;
std::map<std::string, SrcImage*> global_images;

std::vector<std::pair<SrcImage*, SrcImage*>> global_editor_history;

void undo() {
	if (global_editor_history.size() > 0) {
		*(global_editor_history.back().first) = *(global_editor_history.back().second);
		(global_editor_history.back().first)->updateAllClients();
		delete global_editor_history.back().second;
		global_editor_history.pop_back();
	}
}

int loadSequences(libconfig::Config *p_cfg) {
	int status = 0;

	const libconfig::Setting& root = p_cfg->getRoot();

	try {
		const libconfig::Setting &sequences = root["sequences"];
		int count = sequences.getLength();
		for (int i = 0; i < count; ++i)
		{
			const libconfig::Setting &sequence = sequences[i];
			std::string name, sheet;

			if (!(sequence.lookupValue("name", name)
				&& sequence.lookupValue("sheet", sheet)))
				continue;

			std::cout << "  Sequence '" << name << "' on '"
				<< sheet << "'" << std::endl;

			global_sequences[name] = new Sequence();

			const libconfig::Setting &frames = sequence.lookup("frames");
			int cFrames = frames.getLength();
			for (int i = 0; i < cFrames; ++i)
			{
				int x = frames[i][0];
				int y = frames[i][1];
				int w = frames[i][2];
				int h = frames[i][3];
				int d = frames[i][4];
				global_sequences[name]->addEntry(x, y, w, h, d);
			}
		}
		std::cout << std::endl;
	}
	catch (const libconfig::SettingNotFoundException &nfex)
	{
		status = -2;
	}

	try {
		const libconfig::Setting &images = root["images"];
		int count = images.getLength();
		for (int i = 0; i < count; ++i)
		{
			std::string name = images[i];

			global_images[name] = new SrcImage();			

			if (!global_images[name]->loadFromFile(name+".png"))	std::cout << "  Could not load image '" << name << "'" << std::endl;
			global_images[name]->createMaskFromColor(sf::Color(0, 0, 0));			
		}
		std::cout << std::endl;
	}
	catch (const libconfig::SettingNotFoundException &nfex)
	{
		status = -2;
	}
	return status;
}

bool compare_canvas_layers(Canvas* a, Canvas* b);

bool compare_canvas_layers(Canvas* a, Canvas* b) {
	return (a->getLayer()<b->getLayer());
}

class AnimatedObject {
private:
	std::vector<Canvas*> canvases;
public:
	std::map<std::string, Sequence*> allowed_seqs;	
	std::string current_seq_key;	
	
	int frame_counter;
	bool followMouse;	

	AnimatedObject() {
		current_seq_key = "";
		frame_counter = 0;
		followMouse = false;		
	}
	bool nextFrame() {
		bool result = false;
		if (allowed_seqs[current_seq_key]->hasEntryAt(frame_counter)) {
			setTile(allowed_seqs[current_seq_key]->getEntryAt(frame_counter).getTile());
			result = true;
		}
		frame_counter++;
		if (frame_counter > allowed_seqs[current_seq_key]->length)frame_counter = 0;
		return result;
	}
	bool previousFrame() {
		bool result = false;
		frame_counter--;
		if (frame_counter < 0) frame_counter = allowed_seqs[current_seq_key]->length;
		if (allowed_seqs[current_seq_key]->hasEntryAt(frame_counter)) {
			setTile(allowed_seqs[current_seq_key]->getEntryAt(frame_counter).getTile());
			result = true;
		}
		return result;
	}
	void setTile(sf::IntRect tile) {
		for (auto const& value: canvases)
		{
			value->setTextureRect(tile);
		}
	}
	void draw(sf::RenderWindow* window) {
		for (auto const& value : canvases)
		{
			window->draw(*value);
		}
	}
	void setPostion(float x, float y) {
		for (auto const& value : canvases)
		{
			value->setPosition(x, y);
		}
	}
	void setColor(sf::Color color) {
		for (auto const& value : canvases)
		{
			value->setColor(color);
		}
	}
	void mousePressed(sf::Event::MouseButtonEvent* event, Tool *tool) {	
		sf::Vector2f mouse_pos = tool->getPosition();
		if (contains(mouse_pos.x, mouse_pos.y)) {			
			global_editor_history.emplace_back(std::make_pair(canvases[0]->p_srcImage, new SrcImage()));
			*(global_editor_history.back().second) = *(canvases[0]->p_srcImage);
		}
		if (event->button == sf::Mouse::Button::Left && contains(mouse_pos.x, mouse_pos.y)) {
			followMouse = true;			
			canvases[0]->setPixel(mouse_pos.x, mouse_pos.y, tool->getColor());			
		}
		else if (event->button == sf::Mouse::Button::Right && contains(mouse_pos.x, mouse_pos.y)) {				
			tool->setColor(canvases[0]->getPixel(mouse_pos.x, mouse_pos.y));
		}
		else {
			followMouse = false;
		}
	}
	void mouseMove(sf::Event::MouseMoveEvent *event, Tool *tool) {
		if (followMouse) {
			sf::Vector2f mouse_pos = tool->getPosition();			
			canvases[0]->setPixel(mouse_pos.x, mouse_pos.y, tool->getColor());
		}
	}
	void mouseReleased(sf::Event::MouseButtonEvent *event, Tool *tool) {
		followMouse = false;
	}
	bool contains(float x, float y) {
		return canvases[0]->contains(x, y);
	}
	void addCanvas(Canvas* p_canvas) {
		canvases.emplace_back(p_canvas);
		sort();
	}
	void removeCanvas(std::string name) {
		for (std::vector<Canvas*>::iterator it = canvases.begin(); it != canvases.end();++it) {
			if ((*it)->name == name) canvases.erase(it);
		}		
	}
	void sort();	

	
};

void AnimatedObject::sort() {
	std::stable_sort(canvases.begin(), canvases.end(), compare_canvas_layers);
}

int main()
{
	srand(time(NULL));


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// VideoModes
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	int fullscreenModeIndex = 0;
	float fullscreenRatio = (float)modes[fullscreenModeIndex].width / (float)modes[fullscreenModeIndex].height;
	sf::VideoMode fullscreenMode = modes[fullscreenModeIndex];

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Window
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	int window_width = 640;
	int wondow_height = (int)(640.f / fullscreenRatio);
	sf::VideoMode windowedMode = sf::VideoMode(window_width, wondow_height);
	sf::RenderWindow window(windowedMode, "Hugoo Editor");

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// View
	/////////////////////////////////////////////////////////////////////////////////////////////////////	

	float zoom = 2.f;
	sf::View view;
	view.reset(sf::FloatRect(0, 0, window_width / zoom, (float)window_width / (zoom*fullscreenRatio)));
	bool toggleViewFollowPlayer = true;
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
	if (!font.loadFromFile(std::string("src/fonts/FreeMono.ttf").insert(0, mainpath))) {
		char temp;
		std::cin >> temp;
	}
	sf::Text text;
	text.setFont(font); // font is a sf::Font
	text.setCharacterSize(12); // in pixels, not points!
	text.setColor(sf::Color(50, 50, 50));

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// General
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	int tilesize = 16;
	sf::Color bgColorGameMode = sf::Color(170, 190, 240);
	sf::Color bgColorEditMode = sf::Color(100, 100, 100);
	sf::Color* bgColor = &bgColorGameMode;
	bool isEditMode = false;


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// World
	/////////////////////////////////////////////////////////////////////////////////////////////////////	


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game Objects
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	libconfig::Config cfg;
	std::string filename = "sequences.cfg";
	try
	{
		std::cout << "Loading sequences... " << std::endl;
		cfg.readFile(filename.c_str());
	}
	catch (const libconfig::FileIOException &fioex)
	{
		std::cout << "I/O error while reading file." << std::endl;
	}
	catch (const libconfig::ParseException &pex)
	{
		std::cout << "Parse error at " << pex.getFile() << ":" << pex.getLine()
			<< " - " << pex.getError() << std::endl;
	}

	loadSequences(&cfg);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Object Manager
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	Tool tool = Tool(&window);

	Canvas body = Canvas("body");
	body.loadTexture(global_images["hu01_body01"]);

	Canvas pants = Canvas("pants");
	pants.loadTexture(global_images["hu01_pants01"]);
	pants.setLayer(1);

	AnimatedObject human;
	human.allowed_seqs["hu_stand01"] = global_sequences["hu_stand01"];
	human.allowed_seqs["hu_walk01"] = global_sequences["hu_walk01"];
	human.current_seq_key = "hu_walk01";
	human.addCanvas(&body);
	human.addCanvas(&pants);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game Loop
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	//view.setCenter(hugoo.getPosition().x,hugoo.getPosition().y-32);

	while (window.isOpen()) {		
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type)
			{
			case sf::Event::Closed: // Close window : exit
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Escape: // Escape pressed : exit
					window.close();
					break;

				case sf::Keyboard::T: // T pressed : toggle following					
					toggleViewFollowPlayer = !toggleViewFollowPlayer;
					break;
				case sf::Keyboard::F1: // F11 pressed: toggle fullscreen 
					if (!isEditMode) {
						isEditMode = true;
					}
					else {
						isEditMode = false;
					}
					break;
				case sf::Keyboard::F11: // F11 pressed: toggle fullscreen 
					if (!isFullscreen) {
						window.close();
						window.create(fullscreenMode, "Hugoo", sf::Style::Fullscreen);
						window.setFramerateLimit(frameratelimit);
						isFullscreen = true;
					}
					else {
						window.close();
						window.create(windowedMode, "Hugoo");
						window.setFramerateLimit(frameratelimit);
						isFullscreen = false;
					}
					break;
				case sf::Keyboard::F10: //change framerate 
					
					if (frameratelimit == 50) frameratelimit = 1;
					else frameratelimit = 50;
					window.setFramerateLimit(frameratelimit);
					break;
				default:
					break;
				}
				break;
			case sf::Event::KeyReleased:
				std::cout << "keycode: " << event.key.code << std::endl;
				switch (event.key.code) {
				case sf::Keyboard::Right:
					while(!human.nextFrame());
					break;
				case sf::Keyboard::Left:
					while(!human.previousFrame());
					break;
				case sf::Keyboard::S:
					for (std::map<std::string, SrcImage*>::iterator it = global_images.begin(); it != global_images.end(); ++it)
					{
						it->second->saveToFile(it->first + ".png");
						std::cout << "saved: " << it->first + ".png" << std::endl;
					}
					break;
				case sf::Keyboard::Z:
					if (isEditMode) undo();
					break;
				default:
					break;
				}
				break;
			case sf::Event::MouseMoved:
				if(isEditMode) human.mouseMove(&event.mouseMove, &tool);
				break;
			case sf::Event::MouseButtonPressed:
				if (isEditMode) human.mousePressed(&event.mouseButton, &tool);
				break;
			case sf::Event::MouseButtonReleased:
				if (isEditMode) human.mouseReleased(&event.mouseButton, &tool);
				break;
			}
		}
		if (isEditMode) {
			bgColor = &bgColorEditMode;
		}
		else {
			bgColor = &bgColorGameMode;
			human.nextFrame();
		}
		//objectManager.update();


		window.clear(*bgColor);
		if (toggleViewFollowPlayer) { view.setCenter(0, 0); } // replace 0,0 with player position!
				
		human.draw(&window);
				
		totalFrames++;
		elapsedFrames++;
		elapsedTime = clock.getElapsedTime();
		if (elapsedTime.asSeconds() >= 1.f / fpsUpdateFrequency) {
			std::stringstream textStream;
			textStream << "fps: " << elapsedFrames / elapsedTime.asSeconds();
			text.setString(textStream.str());
			elapsedFrames = 0;
			sf::Time elapsedTime = clock.restart();
		}
		window.setView(window.getDefaultView());
		window.draw(text);
		window.setView(view);

		window.display();
	}
	return 0;
}
