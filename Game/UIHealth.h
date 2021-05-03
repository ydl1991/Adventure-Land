#pragma once
#include "SDL_rect.h"
struct SDL_Texture;
struct SDL_Renderer;
class Player;

class UIHealth
{
	Player* m_pPlayer;							// a reference of the object showing health here
	SDL_Texture* m_pHealthIcon;						// a icon represent the health property name
	SDL_Rect m_IconRect;							// a rect represent the location of the health icon
	SDL_Rect m_healthBarBase;						// a grey bar represent the base of the health bar
	SDL_Rect m_dynamicHealthBar;					// a red bar represent the current health
	SDL_Color m_baseColor;							// a grey color
	SDL_Color m_HealthColor;						// a red color

public:
	UIHealth(Player* pObject, SDL_Texture* pHealthIcon);
	~UIHealth();

	// update health and render
	void Update(float deltaTime);
	void Render(SDL_Renderer* pRenderer);

	// Set Rect Locations
	void SetIconRect(int x, int y);
	void SetBaseRect(int x, int y );
	void SetHealthRect(int x, int y);
};

