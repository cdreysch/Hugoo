#include "SrcImage.h"
#include "Canvas.h"


void SrcImage::addClientCanvas(std::string name, Canvas* p_canvas) {
	clientCanvases[name] = p_canvas;
}


void SrcImage::removeClientCanvas(std::string name) {
	clientCanvases.erase(name);
}


void SrcImage::updateAllClients() {
	for (clientCanvases_it = clientCanvases.begin(); clientCanvases_it != clientCanvases.end(); clientCanvases_it++)
	{
		clientCanvases_it->second->loadTexture(this);
	}
}