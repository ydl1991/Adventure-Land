#include "WorldBackground.h"
#include "SDL.h"
#include "SDL_rect.h"
#include "Vector2D.h"

WorldBackground::WorldBackground(SDL_Texture* pTexture, float x, float y, float w, float h)
	: m_camaraOnBackground{x, y, w, h}
	, m_pTexture{ pTexture }
	, m_cameraRollingRate{ 0.01f }
{
	//
}

WorldBackground::~WorldBackground()
{
	//
	m_pTexture = nullptr;
}

void WorldBackground::Update(float deltaTime)
{
	// check if reach the end of the world
	if (m_camaraOnBackground.y > 0)
		m_camaraOnBackground.y -= m_cameraRollingRate;
	else
		m_camaraOnBackground.y = 0;
}

void WorldBackground::Render(SDL_Renderer* pRenderer)
{
	SDL_Rect rect = { (int)(m_camaraOnBackground.x), (int)(m_camaraOnBackground.y), (int)(m_camaraOnBackground.w), (int)(m_camaraOnBackground.h) };
	SDL_RenderCopy(pRenderer, m_pTexture, &rect, nullptr);
}

// Set map location function for level loading
void WorldBackground::SetCameraLocation(Vector2D cameraLocation)
{
	m_camaraOnBackground.x = cameraLocation.m_x;
	m_camaraOnBackground.y = cameraLocation.m_y;
}
