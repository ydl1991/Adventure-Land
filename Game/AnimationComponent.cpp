#include "AnimationComponent.h"
#include "SDL_image.h"
#include "TextureSpriteSheet.h"
#include <iostream>

AnimationComponent::AnimationComponent(TextureSpriteSheet* spriteSheet, float frameRate, int frameWidth, int frameHeight, int frameCount)
	: m_frameRate{frameRate}
	, m_pSpriteSheetTexture{ spriteSheet }
	, m_currentFrameNumber{ -1 }
	, m_currentSequenceName{""}
	, m_nextFrameTime{ 1 / m_frameRate }
	, m_currentFrameTransformOnSheet{}
	, m_transformToShowOnWindow{}
{
	// calculate column count by dividing total width by frame width
	m_spriteSheetColumns = spriteSheet->m_w / frameWidth;

	// simple error check to make sure that the sprite sheet is big enough
	// to hold the designated number of frames
	int rows = spriteSheet->m_h / frameHeight;
	if (rows * m_spriteSheetColumns < frameCount)
	{
		return;
	}

	// init the transform to match the frame dimentions
	m_transformToShowOnWindow.x = 0;
	m_transformToShowOnWindow.y = 0;
	m_transformToShowOnWindow.w = frameWidth;
	m_transformToShowOnWindow.h = frameHeight;

	// init the source rectangular
	m_currentFrameTransformOnSheet = m_transformToShowOnWindow;
}

AnimationComponent::~AnimationComponent()
{
	//
	m_pSpriteSheetTexture->m_pTextureSpriteSheet = nullptr;
	delete m_pSpriteSheetTexture;
	m_pSpriteSheetTexture = nullptr;
}

void AnimationComponent::AddAnimationSequence(const char* sequenceName, int firstFrame, int lastFrame)
{ 
	// std::vector<int> vect = { 2, 1 };
	m_allAnimations[sequenceName] = AnimationSequence{ sequenceName, firstFrame, lastFrame };
}

void AnimationComponent::PlayAnimation(const char* sequenceName)
{
	if (sequenceName == m_currentSequenceName || m_allAnimations.find(sequenceName) == m_allAnimations.end())
	{
		// print error message and dip!
		return;
	}

	AnimationSequence& newSequence = m_allAnimations[sequenceName];
	m_currentFrameNumber = newSequence.m_firstFrame;

	m_currentSequenceName = sequenceName;
	ResetFrameTime();
	UpdateSourceTransform();
}

void AnimationComponent::Update(float deltaTime)
{
	if (m_currentFrameNumber == -1)
	{
		return;
	}

	AnimationSequence& currentSequence = m_allAnimations[m_currentSequenceName];

	bool isFrameDirty = false;

	while (deltaTime > m_nextFrameTime)
	{
		deltaTime -= m_nextFrameTime;
		ResetFrameTime();

		isFrameDirty = true;

		if (currentSequence.m_lastFrame == m_currentFrameNumber)
		{
			m_currentFrameNumber = currentSequence.m_firstFrame;
		}
		else
		{
			++m_currentFrameNumber;
		}
	}

	m_nextFrameTime -= deltaTime;

	if (isFrameDirty)
	{
		UpdateSourceTransform();
	}
}

void AnimationComponent::Render(SDL_Renderer* pRenderer, float x, float y, float w, float h, float rotate)
{
	// render the animation in the correct size
	m_transformToShowOnWindow.x = (int)(x);
	m_transformToShowOnWindow.y = (int)(y);
	m_transformToShowOnWindow.w = (int)(w);
	m_transformToShowOnWindow.h = (int)(h);
	SDL_RenderCopyEx(pRenderer, m_pSpriteSheetTexture->m_pTextureSpriteSheet, &m_currentFrameTransformOnSheet, &m_transformToShowOnWindow, rotate, NULL, SDL_FLIP_NONE);
}

void AnimationComponent::Render(SDL_Renderer* pRenderer)
{
	SDL_RenderCopyEx(pRenderer, m_pSpriteSheetTexture->m_pTextureSpriteSheet, &m_currentFrameTransformOnSheet, nullptr, NULL, NULL, SDL_FLIP_NONE);
}

void AnimationComponent::ResetFrameTime()
{
	m_nextFrameTime = 1 / m_frameRate;
}

void AnimationComponent::UpdateSourceTransform()
{
	// y is the colume, x is the row
	int frameX = m_currentFrameNumber % m_spriteSheetColumns;
	int frameY = m_currentFrameNumber / m_spriteSheetColumns;

	m_currentFrameTransformOnSheet.x = frameX * m_currentFrameTransformOnSheet.w;
	m_currentFrameTransformOnSheet.y = frameY * m_currentFrameTransformOnSheet.h;
}

