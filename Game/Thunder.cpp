#include "Thunder.h"
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
#include "EnemyBoss.h"
#include "Enemy.h"

Thunder::Thunder(EnemyBoss* pBoss, TextureSpriteSheet* pSpriteSheet, CollisionManager* pCManager, GameRunningState* pGameRunningState)
	: m_pOwner{ pBoss }
	, m_isActive{false}
{
	m_pGameRunningState = pGameRunningState;
	// set projectile property
	m_dimentions = { 100, 200 };
	float dx, dy;
	dx = m_pOwner->GetX() + ((m_pOwner->GetWidth() - m_dimentions.m_x) / 2.f);
	dy = m_pOwner->GetY() + m_pOwner->GetHeight() + 1.f;
	m_coordinates = { dx,dy };
	m_objectType = ObjectBase::ObjectType::kBossWeapon;

	//m_direction = { 0, -1 };
	//m_speed = 1500.f;
	//m_health = 1.f;
	AddAnimationComponent(new AnimationComponent(pSpriteSheet, 20, 200, 400, 18));
	m_collisionBox = { (int)dx, (int)dy, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, pCManager, CollisionComponent::CollisionState::kCanBeOverlap));

	// make it Validate and Add collision component
	Validate();
}

Thunder::~Thunder()
{
	// make it Invalid and delete collision component
	Invalidate();
}

void Thunder::Init()
{
	// for things need to be initialize out side constructor
	m_pAnimationComponent->AddAnimationSequence("Thunder", 0, 17);
	m_pAnimationComponent->PlayAnimation("Thunder");
}

void Thunder::Tick(float deltaTime)
{
	// check validation
	if (!m_isValid)
		return;

	// update new postion
	UpdatePosition(deltaTime);
	// update collision area position
	m_pCollisionComponent->SetPosition(m_coordinates);

	// update animation component
	if (m_isActive)
		m_pAnimationComponent->Update(deltaTime);
}

void Thunder::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	// check validation
	if (!m_isActive)
		return;

	if (!m_isValid)
		return;

	if (pOtherCollider && pOtherCollider->IsValid())
	{
		ObjectType objectType = pOtherCollider->GetType();

		switch (objectType)
		{
		case ObjectBase::ObjectType::kPlayer:
			pOtherCollider->Invalidate();
			break;
		case ObjectBase::ObjectType::kWall:
			pOtherCollider->Invalidate();
			break;
		default:
			break;
		}
	}
}

void Thunder::Render(SDL_Renderer* pRenderer)
{
	// check validation
	if (!m_isActive)
		return;

	if (!m_isValid)
		return;

	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), 0.f);
}

void Thunder::UpdatePosition(float deltaTime)
{
	float dx, dy;
	dx = m_pOwner->GetX() + ((m_pOwner->GetWidth() - m_dimentions.m_x) / 2.f);
	dy = m_pOwner->GetY() + m_pOwner->GetHeight() + 1.f;
	m_coordinates.m_x = dx;
	m_coordinates.m_y = dy;
}
