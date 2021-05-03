#pragma once
#include <vector>

class Vector2D;
class CollisionComponent;

/***********************************************************/
// Collision Manager stores all the collision component
// from every object and detect any collision event
/***********************************************************/
class CollisionManager
{
private:
	std::vector<CollisionComponent*> m_activeColliders;
	std::vector<std::pair<CollisionComponent*, CollisionComponent*>> m_collidingPair;

	size_t m_activeColliderSize;

public:
	CollisionManager();
	~CollisionManager();

	// registers a collider as active, so it will be checked against
	void AddActiveCollider(CollisionComponent* pCollider);

	// unregisters a collider as active so it will no longer be checked
	void RemoveCollider(CollisionComponent* pCollider);

	// check collision
	bool CheckForCollisionAndNotify(CollisionComponent* pColliderToCheck, Vector2D& movableDelta);

	// check Collision Hit
	bool CheckCollisionOnly(CollisionComponent* pColliderToCheck, Vector2D& objectCoordinates);

	// register colliding pair
	void AddPair(CollisionComponent* pCollider, CollisionComponent* pOther);

	// unregister colliding pair
	void RemoveInvalidPair(CollisionComponent* pCollider);

	// remove all collider and pair
	void RemoveAllColliderAndPair();

	// process colliding pair
	void ProcessCollidingPair();
};

