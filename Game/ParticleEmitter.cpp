#include "ParticleEmitter.h"
#include "ObjectBase.h"
#include "RandomNumberGenerator.h"

// you can directly call and assign base class constructor here, 
// Advantage, if the base class variable is a private variable and you cannot directly access to it
ParticleEmitter::ParticleEmitter(ObjectBase* pOwner, ParticleInfo info)
	: m_pOwner{ pOwner }
	, m_color{ info.m_color}
	, m_side{ info.m_side }
	, m_speed{ info.m_speed }
	, m_center{ info.m_center }
	, m_radius{ info.m_radius }
	, m_numParticles{ info.m_numParticles }
	, m_width{ info.m_width }
	, m_height{ info.m_height }
{
	// Init Particle Vector
	m_particles.reserve(m_numParticles);
	Init(info.m_moveFrom, info.m_moveTo);
}

ParticleEmitter::~ParticleEmitter()
{

	ClearAllParticles();

	delete m_pRNG;
	m_pRNG = nullptr;
}

//-----------------------------------
//	Init Particles
//-----------------------------------
void ParticleEmitter::Init(float rangeFrom, float rangeTo)
{
	// Set rng range from -1 to 1
	m_pRNG = new RandomNumberGenerator(rangeFrom, rangeTo, IntOrFloat::kFloat);

	// Create Particles
	for (int i = 0; i < m_numParticles; ++i)
	{
		Particle* pParticle = new Particle();

		// #DG: this can simply be done by changing the value range in direction through the initialization parameters, the EffectSide isn't strictly necessary. 
		if (m_side == ParticleEmitter::EffectSide::kCircle)
		{
			pParticle->m_direction = Vector2D((m_pRNG->GenerateRandomFloat()), (m_pRNG->GenerateRandomFloat()));
		}
		else if (m_side == ParticleEmitter::EffectSide::kUpperHalf)
		{
			pParticle->m_direction = Vector2D((m_pRNG->GenerateRandomFloat()), -abs(m_pRNG->GenerateRandomFloat()));
		}
		else if (m_side == ParticleEmitter::EffectSide::kLowerHalf)
		{
			pParticle->m_direction = Vector2D((m_pRNG->GenerateRandomFloat()), abs(m_pRNG->GenerateRandomFloat()));
		}

		m_particles.emplace_back(pParticle);
		pParticle = nullptr;
	}
}

//-----------------------------------
//	Clear all 
//-----------------------------------
void ParticleEmitter::ClearAllParticles()
{
	for (auto particle : m_particles)
	{
		delete particle;
		particle = nullptr;
	}
	m_particles.clear();
}

//-----------------------------------
//	Update
//-----------------------------------
void ParticleEmitter::Update(float deltaTime)
{
	float sqRadius = m_radius * m_radius;

	// Loop over particles and update position
	for (int i = 0; i < m_numParticles; ++i)
	{
		Vector2D velocity = { m_particles[i]->m_direction.m_x * m_speed, m_particles[i]->m_direction.m_y * m_speed };
		velocity.ScalarMultiply(deltaTime);

		m_particles[i]->m_positionOffset.Add(velocity);

		// if particle is past the edge of the system radius, recycle it.
		if (m_particles[i]->m_positionOffset.GetSqrMagnitude(0)/* make this a function in Vector2D*/ > sqRadius/* save this as a variable */)
		{
			// this means reset
			m_particles[i]->m_positionOffset = {};
		}
	}

	// Update Emitter location
	Vector2D coordinate = m_pOwner->GetCoordinateVector();
	Vector2D dimention = m_pOwner->GetDimention();
	m_center = { coordinate.m_x + (dimention.m_x / 2.f), coordinate.m_y + (dimention.m_y / 2.f) };
}

//-----------------------------------
//	Render
//-----------------------------------
void ParticleEmitter::Render(SDL_Renderer* pRenderer)
{
	SDL_SetRenderDrawColor(pRenderer, m_color.r, m_color.g, m_color.b, m_color.a);

	// Render all particles with a solid color
	for (int i = 0; i < m_numParticles; ++i)
	{
		auto particle = m_particles[i];
		Vector2D& position = particle->m_positionOffset;

		SDL_Rect particleTransform;
		particleTransform.x = (int)(m_center.m_x + position.m_x);
		particleTransform.y = (int)(m_center.m_y + position.m_y);
		particleTransform.h = m_height;
		particleTransform.w = m_width;

		SDL_RenderFillRect(pRenderer, &particleTransform);
	}
}

//-----------------------------------
//	change partcile velocity
//-----------------------------------
void ParticleEmitter::ChangeParticleSpeed(float speed)
{
	// save the changed speed
	m_speed = speed;
}

//-----------------------------------
//	change partcile radius
//-----------------------------------
void ParticleEmitter::ChangeParticleRadius(float radius)
{
	// save the changed radius
	m_radius = radius;
}

//-----------------------------------
//	change partcile number
//-----------------------------------
void ParticleEmitter::ChangeParticleNumber(size_t numOfParticles)
{
	if (numOfParticles < 0 || numOfParticles == m_numParticles)
		return;

	// set particle number
	m_numParticles = numOfParticles;

	const size_t currentTotalParticle = m_particles.size();
	// if new particle number is greater than old one
	if (numOfParticles > currentTotalParticle)
	{
		const size_t extraNumOfParticles = numOfParticles - currentTotalParticle;

		// create extra particles
		for (size_t i = 0; i < extraNumOfParticles; ++i)
		{
			Particle* pParticle = new Particle();

			if (m_side == ParticleEmitter::EffectSide::kCircle)
			{
				pParticle->m_direction = Vector2D((m_pRNG->GenerateRandomFloat()), (m_pRNG->GenerateRandomFloat()));
			}
			else if (m_side == ParticleEmitter::EffectSide::kUpperHalf)
			{
				pParticle->m_direction = Vector2D((m_pRNG->GenerateRandomFloat()), -abs(m_pRNG->GenerateRandomFloat()));
			}
			else if (m_side == ParticleEmitter::EffectSide::kLowerHalf)
			{
				pParticle->m_direction = Vector2D((m_pRNG->GenerateRandomFloat()), abs(m_pRNG->GenerateRandomFloat()));
			}

			m_particles.emplace_back(pParticle);
			pParticle = nullptr;
		}
	}
}
