#include "ObjectBase.h"
#include "SDL.h"
#include "SDL_image.h"
#include "EventSystem.h"
#include "StateMachine.h"
#include "GameState.h"
#include "GameRunningState.h"
#include "CollisionManager.h"
#include <iostream>

ObjectBase::ObjectBase()
	: m_coordinates{0,0}
	, m_dimentions{0,0}
	, m_direction{0,0}
	, m_health{0}
	, m_maxHealth{0}
	, m_speed{0}
	, m_pCollisionComponent{nullptr}
	, m_pAnimationComponent{nullptr}
	, m_pObjectStateMachine{nullptr}
	, m_pGameRunningState{nullptr}
	, m_isValid{true}
	, m_collisionBox{}
	, m_objectType{}
{
}

ObjectBase::~ObjectBase()
{
	// release memory for animation component
	delete m_pAnimationComponent;
	m_pAnimationComponent = nullptr;
	// release memory for collision component
	delete m_pCollisionComponent;
	m_pCollisionComponent = nullptr;
	// release memory for object statemachine
	delete m_pObjectStateMachine;
	m_pObjectStateMachine = nullptr;
}

// call to check health
void ObjectBase::ChangeHealth(float delta)
{
	// change health
	m_health += delta;
}

void ObjectBase::CheckHealthLimit()
{
	// check if enemy health at boundary
	if (m_health > m_maxHealth)
		m_health = m_maxHealth;
	// check if health is less than 0, if yes, set invalid
	else if (m_health <= 0.f)
		this->Invalidate();
}

void ObjectBase::Invalidate()
{
	m_isValid = false;

	m_pGameRunningState->GetWorld()->GetCollisionManager()->RemoveCollider(m_pCollisionComponent);
}

void ObjectBase::Validate()
{
	m_isValid = true;

	m_pGameRunningState->GetWorld()->GetCollisionManager()->AddActiveCollider(m_pCollisionComponent);
}

void ObjectBase::Init()
{
	//
}

void ObjectBase::Tick(float deltaTime)
{
}

void ObjectBase::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	//
}

void ObjectBase::OverlapUpdate(ObjectBase* pOtherCollider)
{
	//
}

void ObjectBase::HandleEndOverlap(ObjectBase* pOtherCollider)
{
	//
}

void ObjectBase::Render(SDL_Renderer* pRenderer)
{
	//
}
