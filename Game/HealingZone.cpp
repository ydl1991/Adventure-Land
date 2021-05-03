#include "HealingZone.h"
#include "SimpleSDLGame.h"
#include "CollisionManager.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include "SoundLoadingSystem.h"
#include "SoundPlayer.h"
#include "Player.h"
#include <iostream>

HealingZone::HealingZone(ObjectSpawnInfo objectSpawnInfo)
{
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;
	// set wall dimention
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = { 300, 300 };
	m_isValid = true;
	m_objectType = ObjectBase::ObjectType::kZone;

	AddAnimationComponent(new AnimationComponent(objectSpawnInfo.m_pSpriteSheet, 20.f, 400, 400, 30));
	m_collisionBox = { (int)m_coordinates.m_x + 50, (int)m_coordinates.m_y + 10, (int)m_dimentions.m_x - 100, (int)m_dimentions.m_y - 20 };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, objectSpawnInfo.m_pCollisionManager, CollisionComponent::CollisionState::kCanBeOverlap));

	// make it valid and add collision component
	Validate();
}

HealingZone::~HealingZone()
{
	//
}

void HealingZone::Init()
{
	// for things need to be initialize out side constructor
	// init animation sequence
	m_pAnimationComponent->AddAnimationSequence("Healing", 0, 29);
	m_pAnimationComponent->PlayAnimation("Healing");
}

void HealingZone::Tick(float deltaTime)
{
	// check if enemy at boundary
	if (GetY() >= m_pGameRunningState->GetWorld()->GetHeight())
	{
		// if yes, set invalid
		this->Invalidate();
	}

	// check validation
	if (!m_isValid)
	{
		return;
	}

	// timer update
	if (m_effectTimer <= 0)
	{
		m_effectTimer = 1.f;

		Player* pPlayer = m_pGameRunningState->GetPlayer();
		float x = m_coordinates.m_x;
		float y = m_coordinates.m_y;
		if (y < 0)
			y = 0;

		Mix_Chunk* pHeal = nullptr;
		m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/HealingZone.wav", pHeal);
		pHeal->volume = 70;
		/*Mix_RegisterEffect(1, nullptr, nullptr, nullptr);
		Uint8 distanceFromPlayer = (Uint8)(pPlayer->GetCoordinateVector().GetMagnitude(0, x, y) / 800 * 255);
		Mix_SetDistance(1, distanceFromPlayer);*/
		m_pGameRunningState->GetSoundPlayer()->PlayChunk(pHeal, false, 0, 1, 1);
		//Mix_UnregisterEffect(1, nullptr);
	}
	else
		m_effectTimer -= deltaTime;

	// update position
	if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 0)
		m_coordinates.m_y += m_pGameRunningState->GetBackground()->GetCameraRollingRate();

	// for things need to be checked every frame
	m_pAnimationComponent->Update(deltaTime);

	// update collision component
	m_pCollisionComponent->SetPosition(m_coordinates);
}

void HealingZone::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
}

void HealingZone::OverlapUpdate(ObjectBase* pOtherCollider)
{
	if (m_effectTimer <= 0)
	{
		pOtherCollider->ChangeHealth(10);
		std::cout << "Object is healing ... ...\n";
		std::cout << "Object's current health: " << pOtherCollider->GetHealth() << "\n";
	}
}

void HealingZone::HandleEndOverlap(ObjectBase* pOtherCollider)
{
}

void HealingZone::Render(SDL_Renderer* pRenderer)
{
	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), 0.f);
}
