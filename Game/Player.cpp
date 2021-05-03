#include "Player.h"
#include "SDL.h"
#include "SimpleSDLGame.h"
#include "SDL_image.h"
#include "EventSystem.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include "Enemy.h"
#include "TextureSpriteSheet.h"
#include "AnimationComponent.h"
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "Dragon.h"
#include "Tree.h"
#include "Zombie.h"
#include "Vector2D.h"
#include "EnemyBoss.h"
#include "FireBall.h"
#include "UISavingLoading.h"
#include "SpawnerSystem.h"
#include <iostream>

Player::Player(ObjectSpawnInfo objectSpawnInfo, std::string pName)
	: m_fireRate{3.5f}
	, m_mouseControlMotion{false}
	, m_readyToFire{false}
	, m_isWinner{false}
	, m_mouseClick{ false }
	, m_mouseHolding{ false }
	, m_mouseX{ 0 }
	, m_mouseY{ 0 }
	, m_weaponLevel{1}
	, m_pName{pName}
{
	// assign game running state
	m_pGameRunningState = objectSpawnInfo.m_pGameRunningState;

	// set player character Rect and dimention
	m_coordinates = objectSpawnInfo.m_spawnLocation;
	m_dimentions = { 60.f, 84.f };

	// Init starting direction to 0,0
	m_direction = { 0,0 };

	// Init starting character properties
	m_speed = 250.f;
	m_health = 100.f;
	m_maxHealth = 100.f;
	m_objectType = ObjectBase::ObjectType::kPlayer;

	// Init starting collision box dimention
	m_collisionBox = { (int)m_coordinates.m_x,(int)m_coordinates.m_y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };

	// Add the object as Event Listener
	m_pGameRunningState->GetWorld()->GetEventSystem()->AddInputEventListener(this);
}

Player::~Player()
{
	// Remove this object from Event Listener List
	m_pGameRunningState->GetWorld()->GetEventSystem()->RemoveInputEventListener(this);

	// make it valid and add collision component
	Invalidate();
}

void Player::WeaponLevelUp()
{
	// increment weapon level
	if (m_weaponLevel < 3)
	{
		++m_weaponLevel;
		m_pGameRunningState->GetSpawnerSystem()->CheckWeaponComboInit();
	}
}

void Player::Init()
{
	//
}

void Player::Init(int type)
{
	// for things need to be initialize out side constructor
	// init animation sequence
	if (type == 0)
	{
		//human character
		m_pAnimationComponent->AddAnimationSequence("Idle", 0, 0);
		m_pAnimationComponent->AddAnimationSequence("Move", 7, 17);
		m_pAnimationComponent->AddAnimationSequence("Fire", 0, 6);
		m_pAnimationComponent->PlayAnimation("Move");
	}
	else
	{
		// warplan character
		m_pAnimationComponent->AddAnimationSequence("Level 1", 0, 2);
		m_pAnimationComponent->AddAnimationSequence("Level 2", 4, 6);
		m_pAnimationComponent->AddAnimationSequence("Level 3", 8, 10);
		m_pAnimationComponent->AddAnimationSequence("Level 4", 12, 14);
		m_pAnimationComponent->PlayAnimation("Level 1");
	}

	// make it valid and add collision component
	Validate();
}

void Player::Tick(float deltaTime)
{
	// check death
	if (!m_isValid)
	{
		return;
	}

	// check postion change
	MotionChange(deltaTime, m_pGameRunningState->GetWorld());

	// check animation property
	m_pAnimationComponent->Update(deltaTime);

	// check collision property
	m_pCollisionComponent->SetPosition(m_coordinates);

	// set limit of health
	if (m_health > m_maxHealth)
		m_health = m_maxHealth;

	if (m_coordinates.m_y > m_pGameRunningState->GetWorld()->GetHeight())
	{
		std::cout << "You Fall off and Die!\n";
		Invalidate();
	}

	if (m_health <= 0.f)
	{
		std::cout << "You got killed!\n";
		Invalidate();
	}
}

void Player::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	//
	if (pOtherCollider && pOtherCollider->IsValid())
	{
		ObjectType objectType = pOtherCollider->GetType();

		if (objectType == ObjectType::kBoss)
		{
			if (pOtherCollider->GetHealth() > 0.f)
			{
				// anything touches the boss becomes invalid
				this->Invalidate();
				return;
			}
		}

		if (objectType == ObjectType::kBossWeapon)
		{
			FireBall* pFireBall = dynamic_cast<FireBall*>(pOtherCollider);
			if (pFireBall)
			{
				ChangeHealth(-50.f);
				// log health after damage
				std::cout << "Player health reduce to " << GetHealth() << "\n";
				return;
			}
		}
	}
}

// render sprite with rotation and animation
void Player::Render(SDL_Renderer* pRenderer)
{
	// calculate vector rotation
	CalculateRotation();
	// if firing projectile, rotation angle become 0
	float rotation = GetRotationAngle();
	if (m_readyToFire)
	{
		rotation = 0;
		m_pAnimationComponent->PlayAnimation("Fire");
	}
	// render player
	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), rotation);
}

void Player::ProcessInputEvent(SDL_Event& evt)
{
	switch (evt.type)
	{
		// keyboard events
	case SDL_KEYDOWN:
		ProcessPressKeyBoardEvent(&evt.key);
		break;
	case SDL_KEYUP:
		ProcessReleaseKeyBoardEvent(&evt.key);
		break;
		// mouse events
	case SDL_MOUSEBUTTONDOWN:
		ProcessMouseClickEvent(&evt.button);
		break;
	case SDL_MOUSEBUTTONUP:
		ProcessMouseReleaseEvent(&evt.button);
		break;
	case SDL_MOUSEMOTION:
		ProcessMouseMotionEvent(&evt.motion);
		break;
		// window events
	default:
		break;
	}
}

void Player::MotionChange(float deltaTime, SimpleSDLGame* pGameWorld)
{
	// normalize vector
	Vector2D tempUnitVector = m_coordinates.NormalizeVector(1
															, m_coordinates.m_x + m_direction.m_x
															, m_coordinates.m_y + m_direction.m_y);

	m_direction.m_x = tempUnitVector.m_x;
	m_direction.m_y = tempUnitVector.m_y;

//////////////////////////////////////////////////////////////////////
// if mouse event trigger, we will normalize vector for mouse moving action
// otherwise follow keyboard moving action
//////////////////////////////////////////////////////////////////////
	if (m_mouseControlMotion)
	{
		// move if distance more than 20.f
		if (m_coordinates.GetSqrMagnitude(1, (float)(m_mouseX), (float)(m_mouseY)) > (20.f * 20.f))
		{
			Vector2D tempUnitVector = m_coordinates.NormalizeVector(1
																	, (float)(m_mouseX)
																	, (float)(m_mouseY));
			m_direction.m_x = tempUnitVector.m_x;
			m_direction.m_y = tempUnitVector.m_y;
		}
		else
		{
			// stop moving if distance less than 20.f
			m_direction.m_x = 0.f;
			m_direction.m_y = 0.f;
			m_mouseControlMotion = false;
			return;
		}
	}

	// calculate player velocity
	float deltaX = m_direction.m_x * (GetSpeed()) * deltaTime;
	float deltaY = m_direction.m_y * (GetSpeed()) * deltaTime;

	// check collision, and correct delta position
	Vector2D deltaPosition(deltaX, deltaY);
	bool canMove = m_pCollisionComponent->TryMoveAndCheckCollision(deltaPosition);

	if(!canMove)
		m_pAnimationComponent->PlayAnimation("Idle");
	else
		m_pAnimationComponent->PlayAnimation("Move");
	// window boundary check
	// if player coordinates not on window edge
	//update our player coordinates tracker (in double)
	ChangeX(deltaPosition.m_x);
	// if player coordinates not on window edge
	//update our player coordinates tracker (in double)
	ChangeY(deltaPosition.m_y);
}

/////////////////////////////////////////////////////////////////
// calculate rotation
// param:
// 1: unit vector x       2: unit vector y
// indicates player's moving direction
//////////////////////////////////////////////////////////////////
void Player::CalculateRotation()
{
	if (m_direction.m_x != 0 || m_direction.m_y != 0)
	{
		// if moving, calculate new rotation angle
		m_coordinates.Rotate(m_direction.m_x, m_direction.m_y);
	}
}

void Player::ProcessPressKeyBoardEvent(SDL_KeyboardEvent* pData)
{
	//Log key name
	const char* keyName = SDL_GetKeyName(pData->keysym.sym);
	if (pData->repeat)
		return;

	switch (pData->keysym.scancode)
	{
		// movement input
	case SDL_SCANCODE_W:
		// #DG: Remove debug statements. These tend to pollute the log
		std::cout << "Successfully Pressed " << keyName << std::endl;
		SetYDirection(-1.f);
		break;
	case SDL_SCANCODE_S:
		std::cout << "Successfully Pressed " << keyName << std::endl;
		SetYDirection(1.f);
		break;
	case SDL_SCANCODE_A:
		std::cout << "Successfully Pressed " << keyName << std::endl;
		SetXDirection(-1.f);
		break;
	case SDL_SCANCODE_D:
		std::cout << "Successfully Pressed " << keyName << std::endl;
		SetXDirection(1.f);
		break;
	case SDL_SCANCODE_UP:
		// fire projectile
		std::cout << "Fireeeeeeeee!!!! " << std::endl;
		FireSwitch();
		break;
	case SDL_SCANCODE_SPACE:
		// Sprint to boost speed to 1.5 multiplier
		ChangePlayerSpeed();
		m_pAnimationComponent->ChangeFrameRate(20.f);
		std::cout << "Successfully Pressed " << keyName << std::endl;
		std::cout << "Booster Activated!" << std::endl;
		break;
		// case to open save and load menu
	default:
		break;
	}
}

void Player::ProcessReleaseKeyBoardEvent(SDL_KeyboardEvent* pData)
{
	if (pData->repeat)
		return;

	switch (pData->keysym.scancode)
	{
		// #DG: this could just use a vector equal to the current move direction, 
		// then at the end of this function replace m_direction with the Vector calcluated here

		// stop moving
	case SDL_SCANCODE_W:
		SetYDirection(0.f);
		break;
	case SDL_SCANCODE_S:
		SetYDirection(0.f);
		break;
	case SDL_SCANCODE_A:
		SetXDirection(0.f);
		break;
	case SDL_SCANCODE_D:
		SetXDirection(0.f);
		break;
	case SDL_SCANCODE_UP:
		FireSwitch();
		m_pAnimationComponent->PlayAnimation("Move");
		break;
	case SDL_SCANCODE_SPACE:
		// deactivate booster, multiplier back to origin
		ChangePlayerSpeed();
		m_pAnimationComponent->ChangeFrameRate(6.f);
		break;
	default:
		break;
	}
}

void Player::ProcessMouseClickEvent(SDL_MouseButtonEvent* pData)
{
	switch (pData->button)
	{
		// motion moving condition
	case SDL_BUTTON_LEFT:
	{
		UISavingLoading* pMenu = m_pGameRunningState->GetWorld()->GetSavingLoadingMenu();

		// if save and load menu is not open
		if (pMenu && pMenu->GetCurrentMenu() == UISavingLoading::Menu::kOff)
		{
			if (!m_mouseClick)
				ChangeMouseClickState();

			if (!GetMotionState())
				ChangeMotionState();

			if (!m_mouseHolding)
				ChangeMouseHoldingState();

			m_mouseX = pData->x;
			m_mouseY = pData->y;
		}
	}
		break;
	default:
		break;
	}
}

void Player::ProcessMouseReleaseEvent(SDL_MouseButtonEvent* pData)
{
	switch (pData->button)
	{
		// reset motion moving, stop player move
	case SDL_BUTTON_LEFT:
		if (m_mouseClick)
			ChangeMouseClickState();

		if (m_mouseHolding)
			ChangeMouseHoldingState();

		break;
	default:
		break;
	}
}

void Player::ProcessMouseMotionEvent(SDL_MouseMotionEvent* pData)
{
	// if not released, reactivate moving
	if (m_mouseClick)
	{
		if (!GetMotionState())
			ChangeMotionState();
	}

	// if still holding the click, record mouse coordinates for moving action
	if (GetMotionState() && m_mouseHolding)
	{
		m_mouseX = pData->x;
		m_mouseY = pData->y;
	}
}

