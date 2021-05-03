#pragma once
#include "ZoneBase.h"
class WinZone : public ZoneBase
{
public:
	WinZone(float x, float y, GameRunningState* pGameRunningState);
	~WinZone();

	// virtual 
	virtual void Init() override;
	virtual void Tick(float deltaTime) override;
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	virtual void Render(SDL_Renderer* pRenderer) override;
};

