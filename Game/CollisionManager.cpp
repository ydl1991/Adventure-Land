#include "CollisionManager.h"
#include "CollisionComponent.h"
#include "SDL.h"
#include "ObjectBase.h"
#include "Player.h"
#include "Vector2D.h"
#include "Projectile.h"
#include <iostream>
#include <algorithm> 
#include <math.h>

CollisionManager::CollisionManager()
	: m_activeColliders{}
	, m_collidingPair{}
	, m_activeColliderSize{0}
{
	//
}

CollisionManager::~CollisionManager()
{
	// clear
	RemoveAllColliderAndPair();
}

void CollisionManager::AddActiveCollider(CollisionComponent* pCollider)
{
	if (pCollider)
	{
		// make sure the object hasn't been added. if not, add to list of active colliders
		if ((std::find(m_activeColliders.begin(), m_activeColliders.end(), pCollider)) == m_activeColliders.end())
		{
			m_activeColliders.emplace_back(pCollider);
			++m_activeColliderSize;
		}
	}
}

void CollisionManager::RemoveCollider(CollisionComponent* pCollider)
{
	if (pCollider)
	{
		// find the invalid collider and remove all the pairs related to it
		RemoveInvalidPair(pCollider);

		// find the collider and remove from active collider
		auto it = m_activeColliders.begin();
		while (it != m_activeColliders.end())
		{
			if ((*it) == pCollider)
			{
				it = m_activeColliders.erase(it);
				--m_activeColliderSize;
			}
			else
			{
				++it;
			}
		}
	}
}

bool CollisionManager::CheckForCollisionAndNotify(CollisionComponent* pColliderToCheck, Vector2D& deltaPosition)
{
	// track if can move
	bool canMove = true;
	
	if (pColliderToCheck)
	{
		// get the collider transform
		SDL_Rect colliderTransform = pColliderToCheck->GetTransform();

		// calculate bounds
		int left = colliderTransform.x;
		int right = colliderTransform.x + colliderTransform.w;
		int top = colliderTransform.y;
		int bottom = colliderTransform.y + colliderTransform.h;

		for (size_t i = 0; i < m_activeColliderSize; ++i)
		{
			auto pOtherCollider = m_activeColliders[i];
			// dont check against ourself!
			if (pOtherCollider == pColliderToCheck)
			{
				continue;
			}

			// get the transform from other collider
			SDL_Rect otherTransform = pOtherCollider->GetTransform();

			// calculate bounds for other collider
			int otherLeft = otherTransform.x;
			int otherRight = otherTransform.x + otherTransform.w;
			int otherTop = otherTransform.y;
			int otherBottom = otherTransform.y + otherTransform.h;

			// check collision
			bool xOverlap = left < otherRight && right > otherLeft;
			bool yOverlap = top < otherBottom && bottom > otherTop;

			if (xOverlap && yOverlap)
			{
				ObjectBase* pObject = pColliderToCheck->GetOwner();
				ObjectBase* pOther = pOtherCollider->GetOwner();

				// if both collision owners are valid objects
				if (pObject->IsValid() && pOther->IsValid())
				{
					// handle begin overlap
					pObject->HandleBeginOverlap(pOther);
					pOther->HandleBeginOverlap(pObject);

					if (pObject->IsValid() && pOther->IsValid())
					{
						// if other object is physically collidable, handle position correction, otherwise overlap through it
						if (pOtherCollider->GetCollisionState() == CollisionComponent::CollisionState::kPhysicallyCollidable)
						{
							canMove = false;
							// correct collision distance for x
							int overlapWidth = std::min(abs(left - otherRight), abs(right - otherLeft));
							int overlapHeight = std::min(abs(bottom - otherTop), abs(top - otherBottom));
							if (overlapWidth > overlapHeight)
							{
								// cast to player to see if player exist
								if (pObject->GetType() == ObjectBase::ObjectType::kPlayer)
								{
									// if player is moving up or staying at the position
									if (deltaPosition.m_y <= 0)
										// offset overlap distance
										deltaPosition.m_y += overlapHeight;
									else
										deltaPosition.m_y -= overlapHeight;
								}
								else
								{
									// if object is moving down
									if (deltaPosition.m_y > 0)
										deltaPosition.m_y -= overlapHeight;
								}
								return canMove;
							}
							else
							{
								// if object is moving left
								if (deltaPosition.m_x < 0)
									// offset overlap distance
									deltaPosition.m_x += overlapWidth;
								else if(deltaPosition.m_x > 0)
									deltaPosition.m_x -= overlapWidth;

								return canMove;
							}
						}
						else
						{
							// add pair for collision update
							AddPair(pColliderToCheck, pOtherCollider);
						}
					}
				}
			}
		}
	}
	return canMove;
}

bool CollisionManager::CheckCollisionOnly(CollisionComponent* pColliderToCheck, Vector2D& objectCoordinates)
{
	// track if can move
	bool isColliding = false;

	if (pColliderToCheck)
	{
		// get the collider transform
		SDL_Rect colliderTransform = pColliderToCheck->GetTransform();

		// calculate bounds
		int left = colliderTransform.x;
		int right = colliderTransform.x + colliderTransform.w;
		int top = colliderTransform.y;
		int bottom = colliderTransform.y + colliderTransform.h;

		for (size_t i = 0; i < m_activeColliderSize; ++i)
		{
			auto pOtherCollider = m_activeColliders[i];
			// dont check against ourself!
			if (pColliderToCheck == pOtherCollider)
			{
				continue;
			}

			ObjectBase* pOther = pOtherCollider->GetOwner();
			ObjectBase::ObjectType objectType = pOther->GetType();

			// don't count projectile
			if (pOther && objectType != ObjectBase::ObjectType::kWeapon)
			{
				if (pOther->IsValid())
				{
					// get the transform from other collider
					SDL_Rect otherTransform = pOtherCollider->GetTransform();

					// calculate bounds for other collider
					int otherLeft = otherTransform.x;
					int otherRight = otherTransform.x + otherTransform.w;
					int otherTop = otherTransform.y;
					int otherBottom = otherTransform.y + otherTransform.h;

					// check collision
					bool xOverlap = left < otherRight && right > otherLeft;
					bool yOverlap = top < otherBottom && bottom > otherTop;

					// if overlap, set colliding to true
					if (xOverlap && yOverlap)
					{
						isColliding = true;
						objectCoordinates.m_y -= otherTransform.h;
					}
				}
			}
		}
	}
	return isColliding;
}

// call to pair collision component
void CollisionManager::AddPair(CollisionComponent* pCollider, CollisionComponent* pOther)
{
	if (pCollider && pOther)
	{
		for (int i = 0; i < m_collidingPair.size(); ++i)
		{
			bool combination1 = (m_collidingPair[i].first == pCollider) && (m_collidingPair[i].second == pOther);
			bool combination2 = (m_collidingPair[i].first == pOther) && (m_collidingPair[i].second == pCollider);
			// if pair exist, do nothing
			if (combination1 || combination2)
			{
				return;
			}
		}

		m_collidingPair.emplace_back(std::make_pair(pCollider, pOther));
	}
}

// call to remove pair
void CollisionManager::RemoveInvalidPair(CollisionComponent* pCollider)
{
	if (pCollider)
	{
		for (auto it = m_collidingPair.begin(); it != m_collidingPair.end(); )
		{
			if (((*it).first == pCollider) || ((*it).second == pCollider))
			{
				it = m_collidingPair.erase(it);
			}
			else
			{
				++it;
			}
		}

		//for (int i = 0; i < m_collidingPair.size(); ++i)
		//{
		//	// find if the removing collision component is involved in any collision pair
		//	if ((m_collidingPair[i].first == pCollider) || (m_collidingPair[i].second == pCollider))
		//	{
		//		// if yes, find its location and remove
		//		auto location = std::find(m_collidingPair.begin(), m_collidingPair.end(), m_collidingPair[i]);
		//		m_collidingPair.erase(location, location);
		//	}
		//}
	}
}

void CollisionManager::RemoveAllColliderAndPair()
{
	// clear
	m_activeColliders.clear();
	m_activeColliderSize = 0;

	m_collidingPair.clear();
}

// call to process colliding pair
void CollisionManager::ProcessCollidingPair()
{
	// check pair overlap
	// get the collider transform
	for(auto it = m_collidingPair.begin(); it != m_collidingPair.end();)
	{
		CollisionComponent* pCollider = (*it).first;
		CollisionComponent* pOther = (*it).second;

		if (pCollider && pOther && pCollider->GetOwner()->IsValid() && pOther->GetOwner()->IsValid())
		{
			SDL_Rect colliderTransform = (*it).first->GetTransform();

			//  get the transform
			int left = colliderTransform.x;
			int right = colliderTransform.x + colliderTransform.w;
			int top = colliderTransform.y;
			int bottom = colliderTransform.y + colliderTransform.h;

			// get the transform from other collider
			SDL_Rect otherTransform = (*it).second->GetTransform();

			// calculate bounds for other collider
			int otherLeft = otherTransform.x;
			int otherRight = otherTransform.x + otherTransform.w;
			int otherTop = otherTransform.y;
			int otherBottom = otherTransform.y + otherTransform.h;

			// check collision
			bool xOverlap = left < otherRight && right > otherLeft;
			bool yOverlap = top < otherBottom && bottom > otherTop;

			ObjectBase* pFirstObject = (*it).first->GetOwner();
			ObjectBase* pSecondObject = (*it).second->GetOwner();

/*			if (pFirstObject->IsValid() && pSecondObject->IsValid() && xOverlap && yOverlap)
			{

			}
			else */if (pFirstObject->IsValid() && pSecondObject->IsValid() && (!xOverlap || !yOverlap))
			{
				pFirstObject->HandleEndOverlap(pSecondObject);
				pSecondObject->HandleEndOverlap(pFirstObject);
				it = m_collidingPair.erase(it);
				continue;
			}
			else if (!pFirstObject->IsValid() || !pSecondObject->IsValid())
			{
				it = m_collidingPair.erase(it);
				continue;
			}

			pFirstObject->OverlapUpdate(pSecondObject);
			pSecondObject->OverlapUpdate(pFirstObject);
			++it;
		}
		else
		{
			it = m_collidingPair.erase(it);
		}
	}
}
