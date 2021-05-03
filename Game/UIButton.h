#pragma once
#include "SDL_rect.h"
#include <functional>

struct SDL_Renderer;
class AnimationComponent;

/////////////////////////////////////////////////////////////////////
// A clickable button class
/////////////////////////////////////////////////////////////////////
class UIButton
{
	// Position and size.
	SDL_FRect m_transform;

	// Button Animation
	AnimationComponent* m_pAnim;

	// Whether this button is currently highlighted.
	bool m_isHighlighted;

	// Function to be called when this button is clicked.
	std::function<void()> m_callback;

public:
	UIButton(float x, float y, float w, float h, AnimationComponent* pAnim);
	~UIButton();

	// Get Transform
	SDL_FRect& GetTransform() { return m_transform; }
	// Get the animation component
	AnimationComponent* GetAnimation() { return m_pAnim; }

	// Sets this button's highlight set.
	void SetIsHighlighted(bool value) { m_isHighlighted = value; }
	// Sets this button's callback function.
	void SetCallback(std::function<void()> callback) { m_callback = callback; }
	// Set Animation Not Highlighted
	void SetAnimationNotHighlighted(int startFrame, int endFrame);
	// Set Animation Highlighted
	void SetAnimationHighlighted(int startFrame, int endFrame);


	// Moving Button
	void ButtonMoving(float deltaX, float deltaY);

	// Update Button Anim
	void Update(float deltaTime);

	// Renders this object.
	void Render(SDL_Renderer* pRenderer);

	// Triggers the button's callback as if it had been clicked.
	void Trigger();

	// Returns whether the given x,y are within the bounds of the button.
	bool HitTest(int x, int y);
};

