#pragma once
#include "Projectile.h"
class SmallOpticalWave : public Projectile
{
public:
	SmallOpticalWave(ObjectSpawnInfo objectSpawnInfo, Vector2D direction);
	~SmallOpticalWave();

	//// virtual override
	//void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	//void Render(SDL_Renderer* pRenderer) override;
};

