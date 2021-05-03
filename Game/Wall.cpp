#include "Wall.h"
#include "SDL_render.h"
#include "SimpleSDLGame.h"
#include "TextureSpriteSheet.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include "CollisionManager.h"
#include "SoundLoadingSystem.h"
#include "SoundPlayer.h"
#include "EnemyBoss.h"
#include "WallBreakParticle.h"

Wall::Wall(ObjectSpawnInfo objectSpawnInfo)
{
	// set object game state reference
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;

	// set wall dimention and properties
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = { 200, 58 };
	m_isValid = true;
	m_objectType = ObjectBase::ObjectType::kWall;

	// set object animation
	TextureSpriteSheet* pSpriteSheet;
	pSpriteSheet = objectSpawnInfo.m_pSpriteSheet;
	AddAnimationComponent(new AnimationComponent(pSpriteSheet, std::numeric_limits<float>::min(), pSpriteSheet->m_w, pSpriteSheet->m_h, 1));

	// set object collision and check spawn location
	m_collisionBox = { (int)m_coordinates.m_x, (int)m_coordinates.m_y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, objectSpawnInfo.m_pCollisionManager, CollisionComponent::CollisionState::kPhysicallyCollidable));
	objectSpawnInfo.m_pCollisionManager->CheckCollisionOnly(m_pCollisionComponent, m_coordinates);
	// update and correct collision component position
	m_pCollisionComponent->SetPosition(m_coordinates);

	// make it valid and add collision component
	Validate();
}

Wall::~Wall()
{
	//
	CreateCrackedParticle();
	Invalidate();
}

void Wall::Init()
{
	m_pAnimationComponent->AddAnimationSequence("StaticWall", 0, 0);
	m_pAnimationComponent->PlayAnimation("StaticWall");
}

void Wall::Tick(float deltaTime)
{
	// check if wall at boundary
	if (m_coordinates.m_y >= m_pGameRunningState->GetWorld()->GetHeight())
	{
		// if yes, set invalid
		this->Invalidate();
	}

	// check death
	if (!m_isValid)
	{
		return;
	}

	// update position
	if(m_pGameRunningState->GetBackground()->GetCameraDimention().y > 0)
		m_coordinates.m_y += m_pGameRunningState->GetBackground()->GetCameraRollingRate();

	// update collision component
	m_pCollisionComponent->SetPosition(m_coordinates);
}

void Wall::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	if (pOtherCollider && pOtherCollider->IsValid())
	{
		if (pOtherCollider->GetType() == ObjectType::kBoss)
		{
			if (pOtherCollider->GetHealth() > 0)
			{
				// anything touches the boss becomes invalid
				Invalidate();
				Mix_Chunk* wallCrackedSound;
				m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/WallCracked.wav", wallCrackedSound);
				m_pGameRunningState->GetSoundPlayer()->PlayChunk(wallCrackedSound, false, 0);
				return;
			}
		}
	}
}

void Wall::Render(SDL_Renderer* pRenderer)
{
	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), 0.f);
}

void Wall::CreateCrackedParticle()
{
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = m_coordinates;
	objectSpawnInfo.m_dimention = m_dimentions;
	objectSpawnInfo.m_pGameRunningState = m_pGameRunningState;

	m_pGameRunningState->AddObject(new WallBreakParticle(objectSpawnInfo));
}
