#include "SuperBomb.h"
#include "TextureSpriteSheet.h"
#include "ParticleEmitter.h"

SuperBomb::SuperBomb(ObjectSpawnInfo objectSpawnInfo)
	: Item(ItemType::kClearOutScreen)
{
	// Init properties
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_objectType = ObjectBase::ObjectType::kItem;

	// Init animation
	TextureSpriteSheet* pSpriteSheet = objectSpawnInfo.m_pSpriteSheet;
	AddAnimationComponent(new AnimationComponent(pSpriteSheet, std::numeric_limits<float>::min(), pSpriteSheet->m_w, pSpriteSheet->m_h, 1));

	// Init collision
	m_collisionBox = { (int)m_coordinates.m_x, (int)m_coordinates.m_y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, objectSpawnInfo.m_pCollisionManager, CollisionComponent::CollisionState::kCanBeOverlap));

	// Init particle system
	ParticleInfo particleInfo;
	particleInfo.m_center = { m_coordinates.m_x + (m_dimentions.m_x / 2.f), m_coordinates.m_y + (m_dimentions.m_y / 2.f) };
	particleInfo.m_numParticles = 10;
	particleInfo.m_radius = 40.f;
	particleInfo.m_side = ParticleEmitter::EffectSide::kCircle;
	particleInfo.m_moveFrom = -1.f;
	particleInfo.m_moveTo = 1.f;
	particleInfo.m_speed = 30.f;
	particleInfo.m_width = 5;
	particleInfo.m_height = 5;
	particleInfo.m_color = SDL_Color{ 255, 153, 0, 255 };
	m_pParticleEmitter = new ParticleEmitter(this, particleInfo);

	// make it valid and add collision component
	Validate();
}

SuperBomb::~SuperBomb()
{
	delete m_pParticleEmitter;
	m_pParticleEmitter = nullptr;
}
