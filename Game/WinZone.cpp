#include "WinZone.h"
#include "SimpleSDLGame.h"
#include "Player.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include <iostream>

WinZone::WinZone(float x, float y, GameRunningState* pGameRunningState)
{
	m_pGameRunningState = pGameRunningState;
	// set wall dimention
	m_coordinates = { x,y };
	m_dimentions = { 200.f, 200.f };
	m_isValid = true;
	m_collisionBox = { (int)x + 50, (int)y + 10, (int)m_dimentions.m_x - 100, (int)m_dimentions.m_y - 20 };
	m_objectType = ObjectBase::ObjectType::kZone;
}

WinZone::~WinZone()
{
	//
	// make it Invalid and Delete collision component
	Invalidate();
}

void WinZone::Init()
{
	// for things need to be initialize out side constructor
	// init animation sequence
	m_pAnimationComponent->AddAnimationSequence("WinPortal", 0, 7);
	m_pAnimationComponent->PlayAnimation("WinPortal");

	// make it valid and add collision component
	Validate();
}

void WinZone::Tick(float deltaTime)
{
	// check validation
	if (!m_isValid)
	{
		return;
	}

	// update position
	if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 0)
		m_coordinates.m_y += m_pGameRunningState->GetBackground()->GetCameraRollingRate();

	// for things need to be checked every frame
	m_pAnimationComponent->Update(deltaTime);

	// update collision component
	Vector2D collisionPosition{ m_coordinates.m_x + 50, m_coordinates.m_y + 10 };
	m_pCollisionComponent->SetPosition(collisionPosition);
}

// call to handle overlap
void WinZone::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	// check validation
	if (!m_isValid)
		return;

	if (pOtherCollider && pOtherCollider->IsValid())
	{
		if(pOtherCollider->GetType() == ObjectType::kPlayer)
		{
			Player* pPlayer = dynamic_cast<Player*>(pOtherCollider);
			std::cout << "Congrats! You Win The Game!!!\n";
			pPlayer->Win();
		}
	}
}

void WinZone::Render(SDL_Renderer* pRenderer)
{
	// check validation
	if (!m_isValid)
		return;

	m_pAnimationComponent->Render(pRenderer, GetX(), GetY(), GetWidth(), GetHeight(), 0.f);
}
