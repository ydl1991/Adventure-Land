#pragma once
#include "GameEndingState.h"
#include <string>
struct SDL_Texture;
struct SDL_Renderer;
struct TextureSpriteSheet;
class AnimationComponent;
class SimpleSDLGame;
class AnimationObject;
class UITextField;
class SoundPlayer;

class GameWinState : public GameEndingState
{
	// rect and texture
	UITextField* m_pMessageOne;
	AnimationObject* m_pAnimBackground;
	SoundPlayer* m_pSoundPlayer;

public:
	GameWinState(SimpleSDLGame* pGameWorld, int playerType, std::string pName);
	~GameWinState();

	// Game State status
	virtual void EnterState() override;
	virtual void Update(float deltaTime) override;
	virtual void ExitState() override;
	// Function Overload, Event Processing
	virtual bool ProcessEvents() override;
	void ProcessInputEvent(SDL_Event& evt) override;
	// Render
	virtual void RenderCurrentState(SDL_Renderer* pRenderer) override;

private:
	void LoadText();
	void LoadAnimations();
	void LoadBackgroundSounds();

	// Events Processing Functions
	void ProcessEndClickEvent(SDL_MouseButtonEvent* pData);
	void ProcessEndMouseMotionEvent(SDL_MouseMotionEvent* pData);
	void ProcessWindowEvent(SDL_WindowEvent* pData);
};

