#pragma once
#include "ZoneBase.h"
class DamageZone : public ZoneBase
{
public:

	DamageZone(ObjectSpawnInfo objectSpawnInfo);
	~DamageZone();

	// virtual 
	virtual void Init() override;
	virtual void Tick(float deltaTime) override;
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	virtual void OverlapUpdate(ObjectBase* pOtherCollider) override;
	virtual void HandleEndOverlap(ObjectBase* pOtherCollider) override;
	virtual void Render(SDL_Renderer* pRenderer) override;
};

