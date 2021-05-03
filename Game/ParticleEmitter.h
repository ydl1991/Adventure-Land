#pragma once
#include "ComponentBase.h"
#include "Vector2D.h"
#include <SDL.h>
#include <vector>

class ObjectBase;
class RandomNumberGenerator;
struct ParticleInfo;

//------------------------------------------------
//	Particle Effect Component
//------------------------------------------------
class ParticleEmitter : public ComponentBase
{
public:
	enum class EffectSide
	{
		kCircle,
		kUpperHalf,
		kLowerHalf,
	};

	struct Particle
	{
		Vector2D m_positionOffset;
		Vector2D m_direction;

		Particle() : m_positionOffset{}, m_direction{} {}
	};

private:
	std::vector<Particle*> m_particles;

	size_t m_numParticles;
	float m_radius;
	float m_speed;
	int m_width;
	int m_height;
	Vector2D m_center;
	SDL_Color m_color;
	RandomNumberGenerator* m_pRNG;
	ObjectBase* m_pOwner;
	EffectSide m_side;

public:
	ParticleEmitter(ObjectBase* pOwner, ParticleInfo info);
	~ParticleEmitter();

	void Update(float deltaTime) override;
	void Render(SDL_Renderer* pRenderer);

	// Getters
	size_t GetNumOfParticles() const { return m_numParticles; }
	float GetCurrentRadius() const { return m_radius; }
	float GetCurrentSpeed() const { return m_speed; }
	SDL_Color& GetCurrentColor() { return m_color; }

	// Property Variation Functions
	void ChangeParticleSpeed(float speed);
	void ChangeParticleRadius(float radius);
	void ChangeParticleNumber(size_t numOfParticles);

private:
	void Init(float rangeFrom, float rangeTo);
	void ClearAllParticles();
};

//---------------------------------------------
//	Info Package That Initialize Particle Effects
//---------------------------------------------
struct ParticleInfo
{
	size_t m_numParticles;
	float m_radius;
	float m_speed;
	float m_moveFrom;
	float m_moveTo;
	int m_width;
	int m_height;
	Vector2D m_center;
	SDL_Color m_color;
	ParticleEmitter::EffectSide m_side;

	ParticleInfo() = default;
	~ParticleInfo() = default;
};
