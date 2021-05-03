#include "MagicWave.h"
#include "GameRunningState.h"
#include "SimpleSDLGame.h"
#include "SoundLoadingSystem.h"
#include "SoundPlayer.h"
#include "TextureSpriteSheet.h"
#include <SDL_mixer.h>
MagicWave::MagicWave(ObjectSpawnInfo objectSpawnInfo)
{
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;
	// set projectile property
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = { 150.f,60.f };
	m_direction = { 0, -1 };
	m_speed = 800.f;
	m_health = 1.f;
	m_objectType = ObjectBase::ObjectType::kWeapon;

	TextureSpriteSheet* pSpriteSheet = objectSpawnInfo.m_pSpriteSheet;
	AddAnimationComponent(new AnimationComponent(pSpriteSheet, std::numeric_limits<float>::min(), pSpriteSheet->m_w, pSpriteSheet->m_h, 1));
	m_collisionBox = { (int)m_coordinates.m_x, (int)m_coordinates.m_y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, objectSpawnInfo.m_pCollisionManager, CollisionComponent::CollisionState::kCanBeOverlap));

	// make it Invalid and delete collision component
	Invalidate();
}

MagicWave::~MagicWave()
{
	//
}

//void MagicWave::HandleBeginOverlap(ObjectBase* pOtherCollider)
//{
//	// check validation
//	if (!m_isValid)
//		return;
//
//	// for things happen after collision
//	if (pOtherCollider && pOtherCollider->IsValid())
//	{
//		ObjectType objectType = pOtherCollider->GetType();
//
//		// if object is not zone and not other type of projectiles
//		switch (objectType)
//		{
//		case ObjectBase::ObjectType::kEnemy:
//			// kill this object
//			Invalidate();
//			break;
//		case ObjectBase::ObjectType::kBoss:
//			// kill this object
//			Invalidate();
//			break;
//		case ObjectBase::ObjectType::kWall:
//			// kill this object
//			Invalidate();
//			break;
//		default:
//			break;
//		}
//
//		// play sound
//		if (m_pGameRunningState)
//		{
//			// load and play hit sound
//			Mix_Chunk* pHit = nullptr;
//			m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/Hit.wav", pHit);
//			m_pGameRunningState->GetSoundPlayer()->PlayChunk(pHit, false, 0, 1, 4);
//		}
//	}
//}
