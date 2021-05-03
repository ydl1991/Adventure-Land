#pragma once
#include "SDL_rect.h"
#include <functional>

struct SDL_Renderer;
struct SDL_Texture;

class UITextureButton
{
	// Position and size.
	SDL_FRect m_transform;

	// Button Animation
	SDL_Texture* m_pTexture;

	// Function to be called when this button is clicked.
	std::function<void()> m_callback;

public:
	UITextureButton(float x, float y, float w, float h);
	~UITextureButton();

	// Get Transform
	SDL_FRect& GetTransform() { return m_transform; }

	// Sets this button's callback function.
	void SetCallback(std::function<void()> callback) { m_callback = callback; }

	// Set Texture
	void SetTexture(SDL_Texture* pTexture) { m_pTexture = pTexture; }

	// Update Button Anim
	void Update(float deltaTime);

	// Renders this object.
	void Render(SDL_Renderer* pRenderer);

	// Triggers the button's callback as if it had been clicked.
	void Trigger();

	// Returns whether the given x,y are within the bounds of the button.
	bool HitTest(int x, int y);
};

