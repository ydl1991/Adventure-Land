#include "ParticleObject.h"
#include "ParticleEmitter.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include "SimpleSDLGame.h"

ParticleObject::ParticleObject()
	: m_lifetime{}
	, m_pParticleEmitter{nullptr}
{
	//
}

ParticleObject::~ParticleObject()
{
	//
}

void ParticleObject::Tick(float deltaTime)
{
	m_lifetime -= deltaTime;

	if (m_lifetime <= 0 || GetY() > m_pGameRunningState->GetWorld()->GetHeight())
		Invalidate();

	if (m_isValid)
	{
		// position offset regarding to the rolling map
		if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 0)
			m_coordinates.m_y += m_pGameRunningState->GetBackground()->GetCameraRollingRate();
	}
}

void ParticleObject::Render(SDL_Renderer* pRenderer)
{
	// #DG: m_pParticleEmitter needs a null check since it's not created in the constructor and you can't really guarantee it exists when this function is run. 
	if (m_pParticleEmitter && m_isValid)
		m_pParticleEmitter->Render(pRenderer);
}
