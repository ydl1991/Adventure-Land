#include "Zombie.h"
#include "SimpleSDLGame.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include "EventSystem.h"
#include "Projectile.h"
#include "Player.h"
#include "TextureSpriteSheet.h"
#include "RandomNumberGenerator.h"
#include "CollisionManager.h"
#include "StateMachine.h"
#include "Wander.h"
#include "Attack.h"
#include <iostream>
Zombie::Zombie(ObjectSpawnInfo objectSpawnInfo)
{
	// set Enemy character Rect and dimention
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = { 100, 100 };

	// set object properties
	m_speed = 70.f;
	m_health = 100.f;
	m_maxHealth = 200.f;
	m_objectType = ObjectBase::ObjectType::kEnemy;

	// set object animation
	TextureSpriteSheet* pSpriteSheet;
	pSpriteSheet = objectSpawnInfo.m_pSpriteSheet;
	AddAnimationComponent(new AnimationComponent(pSpriteSheet, 6.f, 100, pSpriteSheet->m_h, 6));

	// set object collision and check spawn location
	m_collisionBox = { (int)m_coordinates.m_x, (int)m_coordinates.m_y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, objectSpawnInfo.m_pCollisionManager, CollisionComponent::CollisionState::kCanBeOverlap));
	objectSpawnInfo.m_pCollisionManager->CheckCollisionOnly(m_pCollisionComponent, m_coordinates);
	// update and correct collision component position
	m_pCollisionComponent->SetPosition(m_coordinates);

	// set object system refernces
	m_pEventSystem = objectSpawnInfo.m_pEventSystem;
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;

	// set object original state
	CreateAndInitStateMachine();

	// make it valid and add collision component
	Validate();
}

Zombie::~Zombie()
{
	//
	m_pEventSystem->RemoveNormalEventListener(this);
	CreateDeathParticleObject();
}

void Zombie::Tick(float deltaTime)
{
	CheckHealthLimit();

	// check if enemy at boundary
	if (GetY() >= m_pGameRunningState->GetWorld()->GetHeight())
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
	if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 0)
		m_coordinates.m_y += m_pGameRunningState->GetBackground()->GetCameraRollingRate();

	// state machine to trigger entering state condition
	ObjectStateSwitch();

	// Update current state
	m_pObjectStateMachine->Update(deltaTime);

	// update new postion
	PositionUpdate(deltaTime);

	// update collision component
	m_pCollisionComponent->SetPosition(m_coordinates);

	// check for motion status
	if (m_isResting)
	{
		m_speed = 1;
		m_restTimer -= deltaTime;
		if (m_restTimer <= 0)
		{
			m_speed = 100.f;
			ResumeMoving();
			m_restTimer = 0.5f;
		}
	}

	// for things need to be checked every frame
	m_pAnimationComponent->Update(deltaTime);
}

void Zombie::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	// for things happen after collision
	// overlap with projectile
	if (pOtherCollider && pOtherCollider->IsValid())
	{
		// TODO
		ObjectType objectType = pOtherCollider->GetType();
		if (objectType == ObjectType::kWeapon)
		{
			m_pEventSystem->AddNormalEventListener(this, Event::EventType::kGetHurt);
			GetHit();
			return;
		}

		if (objectType == ObjectType::kPlayer)
		{
			pOtherCollider->ChangeHealth(-20);
			// log health after damage
			std::cout << "Player health reduce to " << pOtherCollider->GetHealth() << "\n";
			Invalidate();
			return;
		}
	}
}

void Zombie::Render(SDL_Renderer* pRenderer)
{
	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), 0.f);
}

void Zombie::PositionUpdate(float deltaTime)
{
	// calculate change in position
	float deltaX = GetXDirection() * GetSpeed() * deltaTime;
	float deltaY = GetYDirection() * GetSpeed() * deltaTime;

	// check collision and correct delta position
	Vector2D deltaPosition(deltaX, deltaY);
	bool canMove = m_pCollisionComponent->TryMoveAndCheckCollision(deltaPosition);

	// change direction if collide
	if (!canMove)
	{
		ChangeDirection();
	}

	if (m_direction.m_y <= 0)
	{
		Vector2D tryDown{ 0, 2.f };
		bool isColliding = m_pCollisionComponent->CheckMoveCollisionOnly(tryDown);
		if (!isColliding)
			m_pEventSystem->AddNormalEventListener(this, Event::EventType::kMoveDown);
	}

	ChangeX(deltaPosition.m_x);
	ChangeY(deltaPosition.m_y);
}

void Zombie::ChangeDirection()
{
	int randomDirection = m_pLeftOrRightRNG->GenerateRandomInt();
	if (randomDirection == 0)
		m_pEventSystem->AddNormalEventListener(this, Event::EventType::kMoveLeft);
	else
		m_pEventSystem->AddNormalEventListener(this, Event::EventType::kMoveRight);
	//else
	//{
	//	m_pEventSystem->AddEvent(this, Event::EventType::kMoveDown);
	//}
}

void Zombie::CreateAndInitStateMachine()
{
	m_pObjectStateMachine = new StateMachine();
	BaseState* pNewState = new Wander(this);
	m_pObjectStateMachine->SetCurrentState(pNewState);
	pNewState = nullptr;
}

void Zombie::ObjectStateSwitch()
{
	// get player
	Player* pPlayer = m_pGameRunningState->GetPlayer();

	if (m_pObjectStateMachine->GetCurrentState()->GetStateType() != BaseState::StateType::kAttack)
	{
		// if player is right below perpendicular to the dragon
		float playerX, playerY, xDifference;
		playerX = pPlayer->GetX();
		playerY = pPlayer->GetY();
		xDifference = (playerX - m_coordinates.m_x) * (playerX - m_coordinates.m_x);

		bool rightBelow = (playerY > m_coordinates.m_y&& xDifference < 25);

		if (m_gotHit || rightBelow)
		{
			BaseState* pNewState = new Attack(this, 2.f);
			m_pObjectStateMachine->SetCurrentState(pNewState);
			pNewState = nullptr;
		}
	}
	else
	{
		if ((pPlayer->GetY() - m_coordinates.m_y) < 0)
		{
			BaseState* pNewState = new Wander(this);
			m_pObjectStateMachine->SetCurrentState(pNewState);
			pNewState = nullptr;
		}
	}
}
