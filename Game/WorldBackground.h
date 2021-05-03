#pragma once
#include "SDL_rect.h"
#include "Background.h"
struct SDL_Texture;
struct SDL_Renderer;
class Vector2D;

class WorldBackground : public Background
{
	// Background
	SDL_FRect m_camaraOnBackground;
	SDL_Texture* m_pTexture;
	float m_cameraRollingRate;

public:
	WorldBackground(SDL_Texture* pTexture, float x, float y, float w, float h);
	~WorldBackground();

	// Update
	void Update(float deltaTime);

	// Renderer
	virtual void Render(SDL_Renderer* pRenderer) override;

	// Getter
	SDL_FRect GetCameraDimention() const { return m_camaraOnBackground; };
	float GetCameraRollingRate() const { return m_cameraRollingRate; };

	// Setter
	void SetCameraLocation(Vector2D cameraLocation);
};

