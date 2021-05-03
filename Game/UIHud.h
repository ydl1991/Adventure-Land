#pragma once
#include "SDL_rect.h"
class GameRunningState;
class UITextField;
class UIHealth;
class UIPlayerInfo;
struct SDL_Texture;
struct SDL_Renderer;

class UIHud
{
	GameRunningState* m_pGameRunningState;					// reference of the game world

	UIPlayerInfo* m_pPlayerInfo;							// a Player Icon and Name display UI
	UIHealth* m_pObjectHealthUI;							// a Health Bar UI

	SDL_Texture* m_pHudLayout;								// a texture represents the hud layout
	SDL_Rect m_hudRect;										// a rect represent the location of the hud layout

public:
	UIHud(GameRunningState* pGameRunningState, int playerType);
	~UIHud();

	// Update HUD and Render
	void Update(float deltaTime);
	void Render(SDL_Renderer* pRenderer);

	// Set Rect Locations
	void SetLayoutRect(int x, int y);

private:
	// initialization function
	void InitUIHealth();
	void InitUIPlayerInfo(int playerType);
	void InitHudLayout();
};

