#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "SDL.h"
#include "SDL_rect.h"
#include "ObjectBase.h"

CollisionComponent::CollisionComponent(ObjectBase* pObject, SDL_Rect transform, CollisionManager* pCollisionManager, CollisionState state)
	: m_pOwner{ pObject }
	, m_transform{ transform }
	, m_pCollisionManager{ pCollisionManager }
	, m_collisionState{ state }
{
	//
	// m_pCollisionManager->AddActiveCollider(this);
}

CollisionComponent::~CollisionComponent()
{
	//
	// m_pCollisionManager->RemoveCollider(this);
	m_pOwner = nullptr;
	m_pCollisionManager = nullptr;
}

void CollisionComponent::Update(float deltaTime)
{
	//
}

void CollisionComponent::SetPosition(Vector2D position)
{
	//
	m_transform.x = (int)(position.m_x);
	m_transform.y = (int)(position.m_y);
}

void CollisionComponent::SetDimention(Vector2D newDimention)
{
	m_transform.w = (int)(newDimention.m_x);
	m_transform.h = (int)(newDimention.m_y);
}

bool CollisionComponent::TryMoveAndCheckCollision(Vector2D& deltaPosition)
{
	// check validation of collision manager
	if(!m_pCollisionManager)
	{ 
		// print error message
		//
		return true;
	}

	// temporarily update postion, then perform the check
	m_transform.x += (int)(deltaPosition.m_x);
	m_transform.y += (int)(deltaPosition.m_y);

	// check collision
	bool canMove = m_pCollisionManager->CheckForCollisionAndNotify(this, deltaPosition);

	return canMove;
}

bool CollisionComponent::CheckMoveCollisionOnly(Vector2D& deltaPosition)
{
	// check validation of collision manager
	if (!m_pCollisionManager)
	{
		// print error message
		//
		return true;
	}

	// temporarily update postion, then perform the check
	m_transform.x += (int)(deltaPosition.m_x);
	m_transform.y += (int)(deltaPosition.m_y);

	// check collision
	bool isColliding = m_pCollisionManager->CheckCollisionOnly(this, deltaPosition);

	return isColliding;
}
