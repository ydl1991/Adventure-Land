#pragma once
#include "ComponentBase.h"
#include "Vector2D.h"
#include "SDL_rect.h"

class ObjectBase;
class CollisionManager;

/***********************************************************/
// a component type class that contains property and methods
// to handles object Collisions
/***********************************************************/
class CollisionComponent : public ComponentBase
{
public:
	enum class CollisionState : uint8_t
	{
		kPhysicallyCollidable = 0,
		kCanBeOverlap,
	};

private:
	// object owning the component
	ObjectBase* m_pOwner;
	SDL_Rect m_transform;
	CollisionManager* m_pCollisionManager;
	CollisionState m_collisionState;

public:
	CollisionComponent(ObjectBase* pObject, SDL_Rect transform, CollisionManager* pCollisionManager, CollisionState state);
	~CollisionComponent();

	// update animation play process
	void Update(float deltaTime) override;

	// Getter
	ObjectBase* GetOwner() const { return m_pOwner; }
	SDL_Rect GetTransform() const { return m_transform; }
	CollisionState GetCollisionState() const { return m_collisionState; };

	void SetPosition(Vector2D position);
	void SetDimention(Vector2D newDimention);

	// attempt to move the collider by the given amount
	// if the move causes collisions, object will be notified
	// returns whether or not the move was successful without collisions
	bool TryMoveAndCheckCollision(Vector2D& deltaPosition);

	bool CheckMoveCollisionOnly(Vector2D& deltaPosition);
};

