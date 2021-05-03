#pragma once
#include "SDL_rect.h"
struct SDL_Texture;
class UITextField;
class Player;
struct SDL_Renderer;

class UIPlayerInfo
{
	Player* m_pPlayer;								// a pointer of the player
	UITextField* m_pPlayerNameDisplay;				// a textfield that displays player name
	SDL_Texture* m_pPlayerIcon;						// a icon represent the health property name
	SDL_Rect m_IconRect;							// a rect represent the location of the health icon

public:
	UIPlayerInfo(Player* pPlayer, SDL_Texture* pPlayerIcon, SDL_Renderer* pRenderer);
	~UIPlayerInfo();

	void Render(SDL_Renderer* pRenderer);

	// Set Rect Locations
	void SetIconRect(int x, int y);

private:
	void Init(SDL_Renderer* pRenderer);
};

