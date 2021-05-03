#pragma once
#include "BaseState.h"
#include <SDL.h>

class GameState : public BaseState
{
public:
	GameState() {};
	virtual ~GameState() {};

	// Game State status
	virtual void EnterState() override {};
	virtual void Update(float deltaTime) override {};
	virtual void ExitState() override {};
	// Function Overload, Event Processing
	virtual bool ProcessEvents() override { return true; };
	// Render
	virtual void RenderCurrentState(SDL_Renderer* pRenderer) {};
};

