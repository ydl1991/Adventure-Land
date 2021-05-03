#pragma once
#include "ObjectBase.h"
#include <SDL.h>
class ParticleEmitter;

// this seems like a good candidate for an abstract class, so that you can't create instances of it. 
class ParticleObject : public ObjectBase
{
protected:
	ParticleEmitter* m_pParticleEmitter;
	float m_lifetime;
	ParticleObject();
public:
	virtual ~ParticleObject();

	// virtual
	void Tick(float deltaTime) override;
	void Render(SDL_Renderer* pRenderer) override;

	// Set life
	void SetLifeTime(float time) { m_lifetime = time; }
};

