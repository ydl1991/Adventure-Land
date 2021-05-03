#pragma once
#include <SDL.h>
#include <map>
#include "ComponentBase.h"

struct TextureSpriteSheet;

/***********************************************************/
// a component type class that contains property and methods
// to handles object animation
/***********************************************************/
class AnimationComponent : public ComponentBase
{
	struct AnimationSequence
	{
		const char* m_name;
		int m_firstFrame;
		int m_lastFrame;
	};

private:
	float m_frameRate;
	int m_spriteSheetColumns;
	TextureSpriteSheet* m_pSpriteSheetTexture;

	// Position of the rect and dimentions
	SDL_Rect m_transformToShowOnWindow;

	// Position and dimentions of the current frame of the sprite sheet
	SDL_Rect m_currentFrameTransformOnSheet;

	// index of the current frame
	int m_currentFrameNumber;

	// Time when the animation should advance to the next frame
	float m_nextFrameTime;

	// current Animation sequence name
	const char* m_currentSequenceName;

	// map of this object's animation sequence
	std::map<const char*, AnimationSequence> m_allAnimations;

public:
	AnimationComponent(TextureSpriteSheet* spriteSheet, float frameRate, int frameWidth, int frameHeight, int frameCount);
	~AnimationComponent();

	// set how many action animation are there
	void AddAnimationSequence(const char* sequenceName, int firstFrame, int lastFrame);
	// set which action animation to play
	void PlayAnimation(const char* sequenceName);
	// update animation play process
	void Update(float deltaTime) override;
	// show animation on screen
	void Render(SDL_Renderer* pRenderer, float x, float y, float w, float h, float rotate);
	void Render(SDL_Renderer* pRenderer);
	// change frame rate
	void ChangeFrameRate(float delta) { m_frameRate = delta; }

private:
	void ResetFrameTime();
	void UpdateSourceTransform();
};