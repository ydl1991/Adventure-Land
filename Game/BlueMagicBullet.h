#pragma once
#include "Projectile.h"
class BlueMagicBullet : public Projectile
{
public:
	BlueMagicBullet(ObjectSpawnInfo objectSpawnInfo);
	~BlueMagicBullet();

	//// virtual override
	//void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	//void Render(SDL_Renderer* pRenderer) override;
};

