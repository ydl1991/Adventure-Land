#pragma once
#include "BaseState.h"

class ObjectState : public BaseState
{
public:
	ObjectState(){};
	virtual ~ObjectState() {};

	// Game State status
	virtual void EnterState() override {};
	virtual void Update(float deltaTime) override {};
	virtual void ExitState() override {};
	// Function Overload, Event Processing
	virtual bool ProcessEvents() override { return true; };
	// Render
	virtual void RenderCurrentState(SDL_Renderer* pRenderer) {};
};