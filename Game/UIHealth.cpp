#include "UIHealth.h"
#include "Player.h"
#include "SDL.h"
#include "SDL_image.h"

//-----------------------------
//	Constructor
//-----------------------------
UIHealth::UIHealth(Player* pObject, SDL_Texture* pHealthIcon)
	: m_pPlayer{ pObject }
	, m_pHealthIcon{ pHealthIcon }
	, m_IconRect{0,0,40,40}
	, m_dynamicHealthBar{0,0,150,20}
	, m_healthBarBase{0,0,150,20}
	, m_baseColor{217,217,217,255}
	, m_HealthColor{255,0,0,255}
{
}

//-----------------------------
//	Destructor
//-----------------------------
UIHealth::~UIHealth()
{
}

//-----------------------------
//	Update Health bar
//-----------------------------
void UIHealth::Update(float deltaTime)
{
	float currentHealth = m_pPlayer->GetHealth();
	float maxHealth = m_pPlayer->GetMaxHealth();

	m_dynamicHealthBar.w = (int)(currentHealth / maxHealth * 150);
}

//----------------------------------------------
//	Render health icon, base bar and health bar
//----------------------------------------------
void UIHealth::Render(SDL_Renderer* pRenderer)
{
	SDL_RenderCopy(pRenderer, m_pHealthIcon, nullptr, &m_IconRect);
	
	SDL_SetRenderDrawColor(pRenderer, m_baseColor.r, m_baseColor.g, m_baseColor.b, m_baseColor.a);
	SDL_RenderFillRect(pRenderer, &m_healthBarBase);
	SDL_SetRenderDrawColor(pRenderer, m_HealthColor.r, m_HealthColor.g, m_HealthColor.b, m_HealthColor.a);
	SDL_RenderFillRect(pRenderer, &m_dynamicHealthBar);
}

//-----------------------------
//	Set icon location if needed
//-----------------------------
void UIHealth::SetIconRect(int x, int y)
{
	m_IconRect.x = x;
	m_IconRect.y = y;
}

//-----------------------------
//	set base bar location
//-----------------------------
void UIHealth::SetBaseRect(int x, int y)
{
	m_healthBarBase.x = x;
	m_healthBarBase.y = y;
}

//-----------------------------
//	set health bar locaation
//-----------------------------
void UIHealth::SetHealthRect(int x, int y)
{
	m_dynamicHealthBar.x = x;
	m_dynamicHealthBar.y = y;
}
