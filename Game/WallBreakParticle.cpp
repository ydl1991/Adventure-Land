#include "WallBreakParticle.h"
#include "ParticleEmitter.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include "SimpleSDLGame.h"

WallBreakParticle::WallBreakParticle(ObjectSpawnInfo objectSpawnInfo)
{
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = objectSpawnInfo.m_dimention;
	m_objectType = ObjectType::kParticle;
	m_lifetime = 1.f;

	// Init particle system
	ParticleInfo particleInfo;
	particleInfo.m_center = { m_coordinates.m_x + (m_dimentions.m_x / 2.f), m_coordinates.m_y + (m_dimentions.m_y / 2.f) };
	particleInfo.m_numParticles = 8;
	particleInfo.m_radius = 60.f;
	particleInfo.m_moveFrom = -0.8f;
	particleInfo.m_moveTo = 0.8f;
	particleInfo.m_width = 10;
	particleInfo.m_height = 10;
	particleInfo.m_side = ParticleEmitter::EffectSide::kLowerHalf;
	particleInfo.m_speed = 600.f;
	particleInfo.m_color = SDL_Color{ 128, 64,0,255 };
	m_pParticleEmitter = new ParticleEmitter(this, particleInfo);

	// make it valid and add collision component
	Validate();
}

WallBreakParticle::~WallBreakParticle()
{
	//
	delete m_pParticleEmitter;
	m_pParticleEmitter = nullptr;
}

void WallBreakParticle::Tick(float deltaTime)
{
	// call base tick first
	ParticleObject::Tick(deltaTime);

	size_t numParticle = m_pParticleEmitter->GetNumOfParticles();
	float parSpeed = m_pParticleEmitter->GetCurrentSpeed();
	float radius = m_pParticleEmitter->GetCurrentRadius();

	// #DG: a good challenge for this kind of behavior would be to do this using only parameters passed into the particle system.
	// Adding a decay rate, and/or acceleration to your particles  would be a good start to creating this kind of behavior
	// Slowly Change Particle Number
	if (m_lifetime < 0.7f && numParticle > 3)
	{
		--numParticle;
		m_pParticleEmitter->ChangeParticleNumber(numParticle);
	}

	// Slowly Change Particle Speed
	if (parSpeed > 100)
	{
		parSpeed -= deltaTime * 800.f;
		m_pParticleEmitter->ChangeParticleSpeed(parSpeed);
	}

	// Slowly Change Particle Radius
	if (radius > 20)
	{
		radius -= deltaTime * 40.f;
		m_pParticleEmitter->ChangeParticleRadius(radius);
	}

	// Update Particle
	m_pParticleEmitter->Update(deltaTime);
}
