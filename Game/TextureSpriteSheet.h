#pragma once
#include <SDL.h>

/***********************************************************/
// Struct that stores sprite sheet information for 
// animation system to use
/***********************************************************/
struct TextureSpriteSheet
{
	SDL_Texture* m_pTextureSpriteSheet;
	int m_w;
	int m_h;
};
