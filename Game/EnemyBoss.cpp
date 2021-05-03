#include "EnemyBoss.h"
#include "GameRunningState.h"
#include "SimpleSDLGame.h"
#include "EventSystem.h"
#include "Projectile.h"
#include "Player.h"
#include "BaseState.h"
#include "TextureSpriteSheet.h"
#include "CollisionManager.h"
#include "WorldBackground.h"
#include "StateMachine.h"
#include "CollisionComponent.h"
#include "Attack.h"
#include "Wander.h"
#include "Angry.h"
#include "DeadFury.h"
#include "StaticBoarder.h"
#include "RandomNumberGenerator.h"
#include "SoundLoadingSystem.h"
#include "ImageLoadingSystem.h"
#include "SoundPlayer.h"
#include "AnimationComponent.h"
#include "GameRunningState.h"
#include "Wall.h"
#include "Thunder.h"
#include "FireBall.h"
#include "IsAngry.h"
#include "IsDeadFury.h"
#include "ManaRestore.h"
#include "HasEnoughMana.h"
#include "AbilityAttack.h"
#include "DeadlyAttack.h"
#include "DtNode.h"
#include "DecisionNode.h"
#include <iostream>
#include <time.h>

EnemyBoss::EnemyBoss(ObjectSpawnInfo objectSpawnInfo)
	: m_attackDuration{ 2.5f }
	, m_isFullyPresent{false}
	, m_mana{100.f}
	, m_manaRegen{0.0005f}
	, m_deathAnimTimer{5.f}
	, m_isCharging{false}
	, m_pThunderAbility{nullptr}
{
	// set Enemy character Rect and dimention
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = { 250, 250 };

	// set object properties
	m_speed = 80.f;
	m_health = 1000.f;
	m_maxHealth = 1000.f;
	m_isAttacking = false;
	m_objectType = ObjectBase::ObjectType::kBoss;

	//set boss animation component
	AddAnimationComponent(new AnimationComponent(objectSpawnInfo.m_pSpriteSheet, 10.f, 300, 326, 80));

	// set object collision and check spawn location
	m_collisionBox = { (int)m_coordinates.m_x, (int)m_coordinates.m_y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };
	AddCollisionComponent(new CollisionComponent(this, m_collisionBox, objectSpawnInfo.m_pCollisionManager, CollisionComponent::CollisionState::kCanBeOverlap));
	objectSpawnInfo.m_pCollisionManager->CheckCollisionOnly(m_pCollisionComponent, m_coordinates);
	// update and correct collision component position
	m_pCollisionComponent->SetPosition(m_coordinates);

	// set object system refernces
	m_pEventSystem = objectSpawnInfo.m_pEventSystem;
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;

	// Create and Init All Boss Animations
	CreateAndInitAllAnimation();

	// Init boss original state
	CreateAndInitStateMachine();

	// build ability tree
	BuildAbilityAttackTree();

	Invalidate();
}

EnemyBoss::~EnemyBoss()
{
	// delete fire shield effect
	delete m_pFireShieldEffect;
	m_pFireShieldEffect = nullptr;

	// delete mana regen effect
	delete m_pManaRegenEffect;
	m_pManaRegenEffect = nullptr;

	delete m_pDeadAnimation;
	m_pDeadAnimation = nullptr;

	delete m_pAttackTreeRootNode;
	m_pAttackTreeRootNode = nullptr;
}

void EnemyBoss::Init()
{
	//set boss animation
	m_pAnimationComponent->AddAnimationSequence("Attack", 0, 39);
	m_pAnimationComponent->AddAnimationSequence("Dead", 40, 49);
	m_pAnimationComponent->AddAnimationSequence("Move", 50, 79);
	m_pAnimationComponent->PlayAnimation("Move");
}

void EnemyBoss::Tick(float deltaTime)
{
	CheckHealthLimitAndDeathAnimTrigger(deltaTime);

	// check death
	if (!m_isValid)
		return;

	// check fully present
	if (!m_isFullyPresent && m_coordinates.m_y > 10)
	{
		m_isFullyPresent = true;
	}

	if (m_health > 0.f)
	{
		// Update current state
		m_pObjectStateMachine->Update(deltaTime);

		// Regenerate mana
		ManaRegeneration();

		// Process movement or attack action
		MoveOrAttack(deltaTime);

		// update and correct collision component position
		m_pCollisionComponent->SetPosition(m_coordinates);

		// state machine to trigger entering state condition
		CheckStateSwitch();
	}
	else
	{
		// move down when dead
		ChangeX(0.f);
		ChangeY(m_pGameRunningState->GetBackground()->GetCameraRollingRate());
		m_pDeadAnimation->Update(deltaTime);
	}

	// check animations update
	m_pAnimationComponent->Update(deltaTime);
	m_pFireShieldEffect->Update(deltaTime);
	if (m_isCharging)
		m_pManaRegenEffect->Update(deltaTime);
}

void EnemyBoss::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	// check death
	if (!m_isValid || m_health <= 0)
	{
		return;
	}

	// for things happen after collision
	// overlap with projectile
	if (pOtherCollider && pOtherCollider->IsValid())
	{
		if (pOtherCollider->GetType() == ObjectBase::ObjectType::kWeapon)
		{
			m_pEventSystem->AddNormalEventListener(this, Event::EventType::kGetHurt);
			return;
		}
	}
}

void EnemyBoss::Render(SDL_Renderer* pRenderer)
{
	// check death
	if (!m_isValid)
		return;

	// render main animation
	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), 0.f);

	if (m_health > 0)
	{
		// render fire shield animation
		m_pFireShieldEffect->Render(pRenderer, GetX() - 100, GetY() - 100, GetWidth() + 200, GetHeight() + 200, 0.f);

		// render mana regen animation
		if(m_isCharging)
			m_pManaRegenEffect->Render(pRenderer, GetX()-50, GetY()-50, GetWidth() + 100, GetHeight() +100, 0.f);
	}
	else
	{
		// render death animation
		m_pDeadAnimation->Render(pRenderer, GetX()-40, GetY()-40, GetWidth()+80, GetHeight()+80, 0.f);
	}

}

void EnemyBoss::PositionUpdate(float deltaTime)
{
	// calculate change in position
	float deltaX = GetXDirection() * GetSpeed() * deltaTime;
	float deltaY = GetYDirection() * GetSpeed() * deltaTime;

	// check collision and correct delta position
	Vector2D deltaPosition(deltaX, deltaY);
	bool canMove = m_pCollisionComponent->TryMoveAndCheckCollision(deltaPosition);

	// after fully present, change direction if collide
	if (m_isFullyPresent)
	{
		float bossLowerBound = m_coordinates.m_y + m_dimentions.m_y;
		int mapLowerBound = m_pGameRunningState->GetWorld()->GetHeight();
		bool atBoundary = (bossLowerBound >= mapLowerBound) || (m_coordinates.m_y <= 0);
		if (!canMove || atBoundary)
		{
			ChangeDirection();
		}
	}

	// modify x and y
	ChangeX(deltaPosition.m_x);
	ChangeY(deltaPosition.m_y);
}

void EnemyBoss::ChangeDirection()
{
	// generate random direction
	Player* pPlayer = m_pGameRunningState->GetPlayer();
	m_direction = m_coordinates.NormalizeVector(1, pPlayer->GetX(), pPlayer->GetY());
}

//---------------------------------------
//	Create Object State Machine
//---------------------------------------
void EnemyBoss::CreateAndInitStateMachine()
{
	m_pObjectStateMachine = new StateMachine();
	BaseState* pNewState = new Wander(this);
	m_pObjectStateMachine->SetCurrentState(pNewState);
	pNewState = nullptr;
}

//---------------------------------------
//	Create Boss Animations
//---------------------------------------
void EnemyBoss::CreateAndInitAllAnimation()
{
	TextureSpriteSheet* pAnim = nullptr;
	SDL_Renderer* pRenderer = m_pGameRunningState->GetWorld()->GetRenderer();
	ImageLoadingSystem* pImageLoadingSystem = m_pGameRunningState->GetWorld()->GetImageLoadingSystem();

	//	Create Boss Fire Shield
	pImageLoadingSystem->Load(pRenderer, "Assets/Boss/fireShield.png", pAnim);
	m_pFireShieldEffect = new AnimationComponent(pAnim, 20.f, 200, 200, 61);
	m_pFireShieldEffect->AddAnimationSequence("Rolling", 0, 60);
	m_pFireShieldEffect->PlayAnimation("Rolling");
	pAnim = nullptr;

	//	Create Boss Mana Regen Effect
	pImageLoadingSystem->Load(pRenderer, "Assets/Boss/manaRegen.png", pAnim);
	m_pManaRegenEffect = new AnimationComponent(pAnim, 30.f, 200, 200, 121);
	m_pManaRegenEffect->AddAnimationSequence("Charging", 0, 120);
	m_pManaRegenEffect->PlayAnimation("Charging");
	pAnim = nullptr;

	//	Create Boss Death Animation
	pImageLoadingSystem->Load(pRenderer, "Assets/Boss/bossDie.png", pAnim);
	m_pDeadAnimation = new AnimationComponent(pAnim, 15.f, 64, 64, 16);
	m_pDeadAnimation->AddAnimationSequence("Die", 0, 15);
	m_pDeadAnimation->PlayAnimation("Die");
	pAnim = nullptr;
}

//---------------------------------------
//	Process Attack Decision Tree
//---------------------------------------
void EnemyBoss::ProcessAttack()
{
	m_pAttackTreeRootNode->ProcessNode(this);
}

//-----------------------------------------------------
// Thunder ability attack
//-----------------------------------------------------
void EnemyBoss::AttackAbility()
{
	if (m_isFullyPresent)
	{
		m_isAttacking = true;
		m_mana -= 50;
		// if thunder object not spawned, spawn it
		if (!m_pThunderAbility)
		{
			// load thunder animation
			TextureSpriteSheet* pThunder = nullptr;
			SDL_Renderer* pRenderer = m_pGameRunningState->GetWorld()->GetRenderer();
			m_pGameRunningState->GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/Boss/thunder.png", pThunder);
			// spawn thunder attack
			m_pThunderAbility = new Thunder(this, pThunder, m_pGameRunningState->GetWorld()->GetCollisionManager(), m_pGameRunningState);
			m_pThunderAbility->Init();
			m_pThunderAbility->SetActiveStatus(true);
			// add thunder to object list
			m_pGameRunningState->AddObject(m_pThunderAbility);
		}
		// else just turn on active to enable animation rendering and collision
		else
		{
			m_pThunderAbility->SetActiveStatus(true);
		}

		// play thunder sound
		Mix_Chunk* pThunderAbilitySound = nullptr;
		m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/thunder.wav", pThunderAbilitySound);
		pThunderAbilitySound->volume = 128;
		m_pGameRunningState->GetSoundPlayer()->PlayChunk(pThunderAbilitySound, false, 0);
		pThunderAbilitySound = nullptr;
	}
}

//-----------------------------------------------------
// Fire balls attack
//-----------------------------------------------------
void EnemyBoss::DeadlyAbilityAttack()
{
	if (m_isFullyPresent)
	{
		m_isAttacking = true;
		m_mana -= 50;

		// calculate center
		float centerX, centerY;
		centerX = m_coordinates.m_x + (m_dimentions.m_x / 2.f);
		centerY = m_coordinates.m_y + (m_dimentions.m_y / 2.f);
		Vector2D newCoord = { centerX , centerY };

		std::vector<FireBall*>& fireballs = m_pGameRunningState->GetFireBalls();

		// reset each fireball's location and validation when recycling
		for (size_t i = 0; i < fireballs.size(); ++i)
		{
			fireballs[i]->SetCoordinateVector(newCoord);
			fireballs[i]->Validate();
		}

		// play fireball shooting sound
		Mix_Chunk* pFireballShootingSound = nullptr;
		m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/shootFireballs.wav", pFireballShootingSound);
		m_pGameRunningState->GetSoundPlayer()->PlayChunk(pFireballShootingSound, false, 0);
		pFireballShootingSound = nullptr;
	}
}

//----------------------------------------------
// Mana Regen Ability Effect that enhances Regen
//----------------------------------------------
void EnemyBoss::ManaRestoreAbility()
{
	if (m_isFullyPresent)
	{
		// Set ability casting flags and properties
		m_isAttacking = true;
		m_isCharging = true;
		SetManaRegen(0.005f);

		// Play sound of mana regen ability
		Mix_Chunk* pManaRegenSound = nullptr;
		m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/manaRegen.wav", pManaRegenSound);
		m_pGameRunningState->GetSoundPlayer()->PlayChunk(pManaRegenSound, false, 0);
		pManaRegenSound = nullptr;
	}
}

//----------------------------------------------
// Every time when respawn (recycle) boss, 
// we reset its property
//----------------------------------------------
void EnemyBoss::ResetProperty()
{
	// reset object properties
	SetXDirection(0);
	SetYDirection(1);
	SetFullyPresent(false);

	m_dimentions = { 250, 250 };
	m_speed = 80.f;
	m_health = 1000.f;
	m_mana = 100.f;
	m_manaRegen = 0.0005f;
	m_deathAnimTimer = 5.f;
	m_isAttacking = false;

	m_pCollisionComponent->SetPosition(m_coordinates);
	m_pCollisionComponent->SetDimention(m_dimentions);
	m_pObjectStateMachine->SetCurrentState(new Wander(this));
}

//----------------------------------------------
// Boss Process Ability Attack Override
//----------------------------------------------
void EnemyBoss::ProcessObjectAttack()
{
	if (IsValid())
	{
		ProcessAttack();
	}
}

//----------------------------------------------
// Boss State switch condition check
//----------------------------------------------
void EnemyBoss::CheckStateSwitch()
{
	BaseState::StateType currentType = GetObjectStateMachine()->GetCurrentState()->GetStateType();

	// calculate boss health percentage
	float healthPercentage = m_health / m_maxHealth;

	// check object state switch condition
	if (currentType != BaseState::StateType::kDeadFury && healthPercentage < 0.3f)
	{
		BaseState* pNewState = new DeadFury(this, 3.f);
		m_pObjectStateMachine->SetCurrentState(pNewState);
		pNewState = nullptr;
	}
	else if (currentType != BaseState::StateType::kAngry && (healthPercentage < 0.6f && healthPercentage >= 0.3f))
	{
		BaseState* pNewState = new Angry(this, 5.f);
		m_pObjectStateMachine->SetCurrentState(pNewState);
		pNewState = nullptr;
	}
	else if (currentType != BaseState::StateType::kWander && healthPercentage >= 0.6f)
	{
		BaseState* pNewState = new Wander(this);
		m_pObjectStateMachine->SetCurrentState(pNewState);
		m_dimentions = { 250, 250 };
		m_pCollisionComponent->SetDimention(m_dimentions);
		m_speed = 80.f;
		m_manaRegen = 0.0005f;
		pNewState = nullptr;
	}
}

//----------------------------------------------
// If attacking, prevent moving 
// otherwise execute moving
//----------------------------------------------
void EnemyBoss::MoveOrAttack(float deltaTime)
{
	if (m_isAttacking)
	{
		// play attack animation
		m_pAnimationComponent->PlayAnimation("Attack");
		// still checks for collision
		Vector2D deltaPosition(0, 0);
		m_pCollisionComponent->TryMoveAndCheckCollision(deltaPosition);
		// count attack timer
		CountAttackingDuration(deltaTime);
	}
	else
	{
		// update new postion
		PositionUpdate(deltaTime);
	}
}

//----------------------------------------------
// Boss Mana Regen
//----------------------------------------------
void EnemyBoss::ManaRegeneration()
{
	// if mana less than 100, rege with current regeneration rate, otherwise reset regen rate
	if (m_mana < 100.f)
	{
		m_mana += m_manaRegen;
	}
	else
	{
		m_mana = 100.f;
	}
}

//----------------------------------------------
// Count Attacking duration and reset attacking
// Property after finish attacking
//----------------------------------------------
void EnemyBoss::CountAttackingDuration(float deltaTime)
{
	m_attackDuration -= deltaTime;
	if (m_attackDuration <= 0)
	{
		// reset all abilities
		m_pAnimationComponent->PlayAnimation("Move");
		m_isAttacking = false;
		m_isCharging = false;
		SetManaRegen(0.0005f);
		if(m_pThunderAbility)
			m_pThunderAbility->SetActiveStatus(false);
		ResetAttackDurationTimer();
	}
}

// build ability tree
void EnemyBoss::BuildAbilityAttackTree()
{
	m_pAttackTreeRootNode = new HasEnoughMana();

	DtNode* pAngryCheck = new IsAngry();
	DtNode* pDeadFuryCheck = new IsDeadFury();

	pAngryCheck->SetLeftChild(new AbilityAttack());
	pAngryCheck->SetRightChild(pDeadFuryCheck);

	pDeadFuryCheck->SetLeftChild(new DeadlyAttack());

	m_pAttackTreeRootNode->SetLeftChild(pAngryCheck);
	m_pAttackTreeRootNode->SetRightChild(new ManaRestore());
}

// Boss Check health limit
void EnemyBoss::CheckHealthLimitAndDeathAnimTrigger(float deltaTime)
{
	// check if enemy health at boundary
	if (m_health > m_maxHealth)
		m_health = m_maxHealth;
	// check if health is less than 0, if yes, set invalid
	else if (m_health <= 0.f)
	{
		m_pAnimationComponent->PlayAnimation("Dead");
		m_health = -9999.f;
		if (m_deathAnimTimer > 0)
		{
			// play thunder sound
			Mix_Chunk* pBossDead = nullptr;
			m_pGameRunningState->GetWorld()->GetSoundLoadingSystem()->Load("Sounds/Chunk/bossDie.wav", pBossDead);
			pBossDead->volume = 15;
			m_pGameRunningState->GetSoundPlayer()->PlayChunk(pBossDead, false, 0);
			pBossDead = nullptr;

			m_deathAnimTimer -= deltaTime;
		}
		else
		{
			Invalidate();
		}
	}	
}
