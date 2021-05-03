#pragma once
#include "SDL.h"

class Background
{
public:
	Background() {};
	~Background() {};

	// Renderer
	virtual void Render(SDL_Renderer* pRenderer) {};
};

