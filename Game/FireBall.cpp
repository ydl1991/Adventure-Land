#include "FireBall.h"
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
#include "Vector2D.h"
#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"

//FireBall::FireBall(float x, float y, TextureSpriteSheet* pSpriteSheet, CollisionManager* pCManager, GameRunningState* pGameRunningState)
//	:m_rotationAngle{ 110.f }
//{
//	m_pGameRunningState = pGameRunningState;
//	// set projectile property
//	m_coordinates = { x,y };
//	m_dimentions = { 20.f, 80.f };
//	m_direction = {0.46f, 0.98f};
//	m_speed = 400.f;
//	m_health = 1.f;
//	AddAnimationComponent(new AnimationComponent(pSpriteSheet, 15, 128, 512, 60));
//	m_collisionBox = { (int)x, (int)y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
//	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, pCManager, CollisionComponent::CollisionState::kCanBeOverlap));
//}

FireBall::FireBall(ObjectSpawnInfo objectSpawnInfo, Vector2D direction)
{
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;
	// set projectile property
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = { 60.f, 70.f };
	m_direction = direction;
	m_speed = 300.f;
	m_health = 1.f;
	m_objectType = ObjectBase::ObjectType::kBossWeapon;

	AddAnimationComponent(new AnimationComponent(objectSpawnInfo.m_pSpriteSheet, 15, 128, 150, 25));
	m_collisionBox = { (int)m_coordinates.m_x + 5, (int)m_coordinates.m_y + 5, (int)m_dimentions.m_x - 5, (int)m_dimentions.m_x - 5 };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, objectSpawnInfo.m_pCollisionManager, CollisionComponent::CollisionState::kCanBeOverlap));

	// make it Invalid and delete collision component
	Invalidate();
}

FireBall::~FireBall()
{
	// make it Invalid and delete collision component
	Invalidate();
}

void FireBall::Init()
{
	// for things need to be initialize out side constructor
	m_pAnimationComponent->AddAnimationSequence("Fireball", 0, 24);
	m_pAnimationComponent->PlayAnimation("Fireball");
}

void FireBall::Tick(float deltaTime)
{
	// for things need to be checked every frame
	// check validation
	if (!m_isValid)
		return;

	// check if projectile at boundary
	if (m_coordinates.m_y <= 0 || m_coordinates.m_y > m_pGameRunningState->GetWorld()->GetHeight())
	{
		// if yes, set invalid
		this->Invalidate();
	}

	// update new postion
	UpdatePosition(deltaTime);

	// for things need to be checked every frame
	m_pAnimationComponent->Update(deltaTime);
	// update collision area position
	m_pCollisionComponent->SetPosition(m_coordinates);
}

void FireBall::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	// check validation
	if (!m_isValid)
		return;

	// for things happen after collision
	if (pOtherCollider && pOtherCollider->IsValid())
	{
		ObjectType objectType = pOtherCollider->GetType();
		if(objectType == ObjectType::kPlayer || objectType == ObjectType::kWall || objectType == ObjectType::kBoarder)
		{
			this->Invalidate();
			if (m_pGameRunningState)
			{
				// load and play hit sound
				Mix_Chunk* pHit = nullptr;
				m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/Hit.wav", pHit);
				m_pGameRunningState->GetSoundPlayer()->PlayChunk(pHit, false, 0, 1, 4);
			}
		}
	}
}

void FireBall::Render(SDL_Renderer* pRenderer)
{
	// check validation
	if (!m_isValid)
		return;

	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), m_direction.m_rotation);
}

void FireBall::UpdatePosition(float deltaTime)
{
	// calculate changing position
	float deltaX = GetXDirection() * GetSpeed() * deltaTime;
	float deltaY = GetYDirection() * GetSpeed() * deltaTime;

	// check collision, and correct delta position
	Vector2D deltaPosition(deltaX, deltaY);
	m_pCollisionComponent->TryMoveAndCheckCollision(deltaPosition);
	ChangeX(deltaPosition.m_x);
	ChangeY(deltaPosition.m_y);
}
