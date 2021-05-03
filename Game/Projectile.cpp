#include "Projectile.h"
#include "SDL.h"
#include "SDL_image.h"
#include "AnimationComponent.h"
#include "ZoneBase.h"
#include "HealingZone.h"
#include "DamageZone.h"
#include "TextureSpriteSheet.h"
#include "CollisionManager.h"
#include "SimpleSDLGame.h"
#include "GameRunningState.h"
#include "SoundLoadingSystem.h"
#include "SoundPlayer.h"
#include "Player.h"

Projectile::Projectile(ObjectSpawnInfo weaponInfo)
{
	m_pGameRunningState = weaponInfo.m_pGameRunningState;
	// set projectile property
	m_coordinates = weaponInfo.m_spawnLocation;
	m_dimentions = { 10, 20 };
	m_direction = { 0, -1 };
	m_speed = 800.f;
	m_health = 1.f;
	m_objectType = ObjectBase::ObjectType::kWeapon;

	TextureSpriteSheet* pSpriteSheet = weaponInfo.m_pSpriteSheet;
	AddAnimationComponent(new AnimationComponent(pSpriteSheet, std::numeric_limits<float>::min(), pSpriteSheet->m_w, pSpriteSheet->m_h, 1));
	m_collisionBox = { (int)m_coordinates.m_x, (int)m_coordinates.m_y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, weaponInfo.m_pCollisionManager, CollisionComponent::CollisionState::kCanBeOverlap));

	// make it Invalid and delete collision component
	Invalidate();
}

Projectile::~Projectile()
{
	//
	// make it Invalid and delete collision component
	Invalidate();
}

void Projectile::Init()
{
	// for things need to be initialize out side constructor
	m_pAnimationComponent->AddAnimationSequence("Idle", 0, 0);
	m_pAnimationComponent->PlayAnimation("Idle");
}

void Projectile::Tick(float deltaTime)
{
	// for things need to be checked every frame
	// check if projectile at boundary
	if (GetY() <= 0)
	{
		// if yes, set invalid
		this->Invalidate();
	}

	// check validation
	if (!m_isValid)
		return;

	// update new postion
	UpdatePosition(deltaTime);

	// update collision area position
	m_pCollisionComponent->SetPosition(m_coordinates);
}

void Projectile::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	// check validation
	if (!m_isValid)
		return;

	// for things happen after collision
	if (!pOtherCollider->IsValid() || !pOtherCollider)
		return;

	ObjectType objectType = pOtherCollider->GetType();

	// load and play hit sound
	Mix_Chunk* pHit = nullptr;
	m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/Hit.wav", pHit);

	// if object is not zone and not other type of projectiles
	switch (objectType)
	{
	case ObjectBase::ObjectType::kEnemy:
	{
		// kill this object
		this->Invalidate();
		// play sound
		m_pGameRunningState->GetSoundPlayer()->PlayChunk(pHit, false, 0, 1, 4);
		break;
	}
	case ObjectBase::ObjectType::kBoss:
	{
		// kill this object
		this->Invalidate();
		m_pGameRunningState->GetSoundPlayer()->PlayChunk(pHit, false, 0, 1, 4);
		break;
	}
	case ObjectBase::ObjectType::kWall:
	{
		// kill this object
		this->Invalidate();
		m_pGameRunningState->GetSoundPlayer()->PlayChunk(pHit, false, 0, 1, 4);
		break;
	}
	default:
		break;
	}
}

void Projectile::Render(SDL_Renderer* pRenderer)
{
	// check validation
	if (!m_isValid)
		return;

	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), m_direction.m_rotation);
}

void Projectile::UpdatePosition(float deltaTime)
{
	// calculate changing position
	float deltaX = GetXDirection() * GetSpeed() * deltaTime;
	float deltaY = GetYDirection() * GetSpeed() * deltaTime;

	// check collision, and correct delta position
	Vector2D deltaPosition(deltaX, deltaY);
	m_pCollisionComponent->TryMoveAndCheckCollision(deltaPosition);

	if (m_isValid)
	{
		ChangeX(deltaX);
		ChangeY(deltaY);
	}
}
