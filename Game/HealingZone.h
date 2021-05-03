#pragma once
#include "ZoneBase.h"
class HealingZone : public ZoneBase
{
public:
	HealingZone(ObjectSpawnInfo objectSpawnInfo);
	~HealingZone();

	// virtual 
	virtual void Init() override;
	virtual void Tick(float deltaTime) override;
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	virtual void OverlapUpdate(ObjectBase* pOtherCollider) override;
	virtual void HandleEndOverlap(ObjectBase* pOtherCollider) override;
	virtual void Render(SDL_Renderer* pRenderer) override;
};

