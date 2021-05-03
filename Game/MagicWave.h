#pragma once
#include "Projectile.h"
class MagicWave : public Projectile
{
public:
	MagicWave(ObjectSpawnInfo objectSpawnInfo);
	~MagicWave();

	//void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
};

