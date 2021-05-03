#include "Item.h"
#include "GameRunningState.h"
#include "SimpleSDLGame.h"
#include "Player.h"
#include "SoundLoadingSystem.h"
#include "SoundPlayer.h"
#include "ParticleEmitter.h"
Item::Item(ItemType type)
	: m_type{ type }
	, m_pParticleEmitter{ nullptr }
{
	m_direction = { 0, 1 };
	m_dimentions = { 50, 50 };
	m_speed = 150.f;
	m_health = 1.f;
}

void Item::Init()
{
	// for things need to be initialize out side constructor
	m_pAnimationComponent->AddAnimationSequence("Idle", 0, 0);
	m_pAnimationComponent->PlayAnimation("Idle");
}

void Item::Tick(float deltaTime)
{
	// for things need to be checked every frame
	// check if projectile at boundary
	if (GetY() > m_pGameRunningState->GetWorld()->GetHeight())
	{
		// if yes, set invalid
		this->Invalidate();
	}

	// check validation
	if (!m_isValid)
	{
		return;
	}

	// update new postion
	UpdatePosition(deltaTime);

	// update collision area position
	m_pCollisionComponent->SetPosition(m_coordinates);

	// update particle emitter
	m_pParticleEmitter->Update(deltaTime);
}

void Item::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	// for things happen after collision
	if (pOtherCollider && pOtherCollider->IsValid())
	{
		ObjectType objectType = pOtherCollider->GetType();
		
		if (objectType == ObjectType::kPlayer)
		{
			Player* pPlayer = dynamic_cast<Player*>(pOtherCollider);

			// if object is not zone and not other type of projectiles
			if (pPlayer)
			{
				// kill this object
				Invalidate();

				// if type is weapon power up
				switch (m_type)
				{
				case Item::ItemType::kWeaponPowerUp:
					pPlayer->WeaponLevelUp();
					break;
				case Item::ItemType::kClearOutScreen:
					m_pGameRunningState->ClearOutScreen();
					break;
				default:
					break;
				}

				// play sound
				if (m_pGameRunningState)
				{
					// load and play hit sound
					Mix_Chunk* pPowerup = nullptr;
					m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/Powerup.wav", pPowerup);
					m_pGameRunningState->GetSoundPlayer()->PlayChunk(pPowerup, false, 0, 1, 4);
				}
			}
		}
	}
}

void Item::Render(SDL_Renderer* pRenderer)
{
	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), 0.f);
	m_pParticleEmitter->Render(pRenderer);
}

void Item::UpdatePosition(float deltaTime)
{
	// calculate changing position
	float deltaX = GetXDirection() * GetSpeed() * deltaTime;
	float deltaY = GetYDirection() * GetSpeed() * deltaTime;

	// check collision, and correct delta position
	//Vector2D deltaPosition(deltaX, deltaY);
	//m_pCollisionComponent->TryMoveAndCheckCollision(deltaPosition);
	ChangeX(deltaX);
	ChangeY(deltaY);
}
