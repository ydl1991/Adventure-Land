#pragma once
#include "Wall.h"
class GameRunningState;

class StaticBoarder :
	public Wall
{
public:
	StaticBoarder(GameRunningState* pRunningState, float x, float y);
	~StaticBoarder() { Invalidate(); };

	void Render(SDL_Renderer* pRenderer) override;
	void Tick(float deltaTime) override;
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
};

