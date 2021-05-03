#pragma once
#include "SDL_rect.h"
#include <SDL_ttf.h>
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Color;

//////////////////////////
// A text field in game
//////////////////////////
class UITextField
{
	// Position and size for rendering.
	SDL_Rect m_transform;

	// Text texture for rendering.
	SDL_Texture* m_pTexture;
public:
	UITextField(int x, int y, TTF_Font* pFont, const char* pText, SDL_Color color, SDL_Renderer* pRenderer);
	~UITextField();

	// Renders this object
	void Render(SDL_Renderer* pRenderer);

	// Setter
};

