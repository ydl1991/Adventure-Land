#include "Enemy.h"
#include "SDL.h"
#include "SDL_image.h"
#include "Projectile.h"
#include "AnimationComponent.h"
#include "SimpleSDLGame.h"
#include "EventSystem.h"
#include "Player.h"
#include "StateMachine.h"
#include "RandomNumberGenerator.h"
#include "GameRunningState.h"
#include "BloodParticle.h"

Enemy::Enemy()
	: m_isResting{false}
	, m_gotHit{false}
	, m_isAttacking{false}
	, m_restTimer{0.5f}
	, m_pEventSystem{nullptr}
{
	// set Enemy character Rect and dimention
	m_direction = { 0,1 };
	m_isValid = true;

	// Init RNGs
	m_pLeftOrRightRNG = new RandomNumberGenerator(0.f, 1.f);
}

Enemy::~Enemy()
{
	//
	delete m_pLeftOrRightRNG;
	m_pLeftOrRightRNG = nullptr;
}

void Enemy::Init()
{
	// for things need to be initialize out side constructor
	m_pAnimationComponent->AddAnimationSequence("Move", 0, 5);
	m_pAnimationComponent->AddAnimationSequence("Rest", 2, 3);
	m_pAnimationComponent->PlayAnimation("Move");
}

void Enemy::Tick(float deltaTime)
{
	//
}

void Enemy::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	//
}

void Enemy::Render(SDL_Renderer* pRenderer)
{
	//
}

// stop moving play resting animation
void Enemy::GetHit()
{
	m_pAnimationComponent->PlayAnimation("Rest");
	m_isResting = true;
	m_gotHit = true;
}

// resume moving after resting time count down
void Enemy::ResumeMoving()
{
	m_pAnimationComponent->PlayAnimation("Move");
	m_isResting = false;
}

// call to update position
void Enemy::PositionUpdate(float deltaTime)
{
	//
}

void Enemy::ChangeDirection()
{
	//
}

void Enemy::ProcessEnemyEvent(Event::EventType evtType)
{
	// if event and owner are valid
	if (IsValid())
	{
		// process events
		switch (evtType)
		{
		case Event::EventType::kMoveUp:
			ProcessObjectMoveUp();
			break;
		case Event::EventType::kMoveDown:
			ProcessObjectMoveDown();
			break;
		case Event::EventType::kMoveLeft:
			ProcessObjectMoveLeft();
			break;
		case Event::EventType::kMoveRight:
			ProcessObjectMoveRight();
			break;
		case Event::EventType::kChase:
			ProcessObjectChasePlayer();
			break;
		case Event::EventType::kAttact:
			ProcessObjectAttack();
			break;
		case Event::EventType::kGetHurt:
			ProcessObjectGetHurt();
			break;
		case Event::EventType::kHeal:
			ProcessObjectHeal();
			break;
		default:
			break;
		}
	}
}

void Enemy::ProcessObjectMoveUp()
{
	// 
	SetXDirection(0);
	SetYDirection(-1);
}

void Enemy::ProcessObjectMoveDown()
{
	SetXDirection(0);
	SetYDirection(1);
}

void Enemy::ProcessObjectMoveLeft()
{
	SetXDirection(-1);
	SetYDirection(0);
}

void Enemy::ProcessObjectMoveRight()
{
	SetXDirection(1);
	SetYDirection(0);
}

void Enemy::ProcessObjectChasePlayer()
{
	if (IsValid())
	{
		// get player reference
		Player* pPlayer = m_pGameRunningState->GetPlayer();
		// check if player reference exist
		if (pPlayer && pPlayer->IsValid())
		{
			// calculate unit vector between enemy and player
			Vector2D unitVector = m_coordinates.NormalizeVector(1, pPlayer->GetX(), pPlayer->GetY());
			// set unit vector to enemy's direction
			SetXDirection(unitVector.m_x);
			SetYDirection(unitVector.m_y);
		}
	}
}

void Enemy::ProcessObjectAttack()
{
	//
}

void Enemy::ProcessObjectGetHurt()
{
	ChangeHealth(-50);
}

void Enemy::ProcessObjectHeal()
{
	ChangeHealth(10);
}

//------------------------------------------
// Add Death Particle Effect at Death Location
//------------------------------------------
void Enemy::CreateDeathParticleObject()
{
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = m_coordinates;
	objectSpawnInfo.m_dimention = m_dimentions;
	objectSpawnInfo.m_pGameRunningState = m_pGameRunningState;

	m_pGameRunningState->AddObject(new BloodParticle(objectSpawnInfo));
}