#pragma once
#include "ParticleObject.h"
class WallBreakParticle : public ParticleObject
{
public:
	WallBreakParticle(ObjectSpawnInfo objectSpawnInfo);
	~WallBreakParticle();

	// virtual
	void Tick(float deltaTime) override;
};

