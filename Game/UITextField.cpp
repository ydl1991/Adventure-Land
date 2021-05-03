#include "UITextField.h"

//-----------------------------
//	Constructor
//-----------------------------
UITextField::UITextField(int x, int y, TTF_Font* pFont, const char* pText, SDL_Color color, SDL_Renderer* pRenderer)
	: m_transform{ }
	, m_pTexture{nullptr}
{
	// Render text to a surface.
	SDL_Surface* pTextSurface = TTF_RenderText_Solid(pFont, pText, color);

	// Create texture from surface.
	m_pTexture = SDL_CreateTextureFromSurface(pRenderer, pTextSurface);

	// Set text field's position.
	m_transform.x = x;
	m_transform.y = y;

	// Set dimensions based on how big the rendered text is.
	m_transform.w = pTextSurface->w;
	m_transform.h = pTextSurface->h;

	// Always free the surface.
	SDL_FreeSurface(pTextSurface);
}

//-----------------------------
//	Destructor
//-----------------------------
UITextField::~UITextField()
{
	SDL_DestroyTexture(m_pTexture);
}

//-----------------------------
//	Render Text
//-----------------------------
void UITextField::Render(SDL_Renderer* pRenderer)
{
	SDL_RenderCopy(pRenderer, m_pTexture, nullptr, &m_transform);
}
