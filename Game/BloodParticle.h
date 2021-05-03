#pragma once
#include "ParticleObject.h"
class BloodParticle : public ParticleObject
{
public:
	BloodParticle(ObjectSpawnInfo objectSpawnInfo);
	~BloodParticle();

	// virtual
	void Tick(float deltaTime) override;
};

