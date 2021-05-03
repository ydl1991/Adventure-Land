#pragma once
#include "SDL_rect.h"
#include "Vector2D.h"
#include "CollisionComponent.h"
#include "AnimationComponent.h"
#include <string>

struct SDL_Renderer;
class GameRunningState;
class CollisionComponent;
class StateMachine;
class EventSystem;
class CollisionManager;
struct TextureSpriteSheet;

// #DG: This is a good use of this programming pattern
struct ObjectSpawnInfo
{
	Vector2D m_spawnLocation;
	Vector2D m_dimention;
	TextureSpriteSheet* m_pSpriteSheet;
	CollisionManager* m_pCollisionManager;
	GameRunningState* m_pGameRunningState;
	EventSystem* m_pEventSystem;

	ObjectSpawnInfo() = default;
	~ObjectSpawnInfo() = default;
};

/***********************************************************/
//              Base Class for Game Objects
/***********************************************************/
class ObjectBase
{
public:
	enum class ObjectType
	{
		kPlayer,
		kEnemy,
		kBoss,
		kZone,
		kItem,
		kWall,
		kBoarder,
		kWeapon,
		kBossWeapon,
		kParticle,
	};

protected:
	ObjectBase();

	// pointer of the game world and game state
	GameRunningState* m_pGameRunningState;
	StateMachine* m_pObjectStateMachine;	// Object StateMachine Instance

	// Coordinates Variables
	Vector2D m_coordinates;
	Vector2D m_dimentions;
	Vector2D m_direction;
	SDL_Rect m_collisionBox;

	// component system
	CollisionComponent* m_pCollisionComponent;
	AnimationComponent* m_pAnimationComponent;

	// object property
	float m_speed;
	float m_health;
	float m_maxHealth;
	bool m_isValid;
	ObjectType m_objectType;

public:
	// public destructor
	virtual ~ObjectBase();

	// Getters
	GameRunningState* GetGameRunningState() const { return m_pGameRunningState; }
	StateMachine* GetObjectStateMachine() const { return m_pObjectStateMachine; }			// Get ObjectStateMachine
	ObjectType GetType() const { return m_objectType; }

	// Collision System related
	void AddCollisionComponent(CollisionComponent* pCollisionComponent) { m_pCollisionComponent = pCollisionComponent; };
	CollisionComponent* GetCollisionSystem() const { return m_pCollisionComponent; };
	SDL_Rect& GetCollisionBox() { return m_collisionBox; };

	// Animation system related
	void AddAnimationComponent(AnimationComponent* pAnimationComponent) { m_pAnimationComponent = pAnimationComponent; };
	AnimationComponent* GetAnimationComponent() const { return m_pAnimationComponent; };

	// Coordinate related functions
	Vector2D& GetCoordinateVector() { return m_coordinates; };
	void SetCoordinateVector(Vector2D newCoord) { m_coordinates = newCoord; }
	Vector2D GetDimention() const { return m_dimentions; };
	float GetX() const { return m_coordinates.m_x; };
	float GetY() const { return m_coordinates.m_y; };
	void ChangeX(float deltaX) { m_coordinates.m_x += deltaX; };
	void ChangeY(float deltaY) { m_coordinates.m_y += deltaY; };
	float GetWidth() const { return m_dimentions.m_x; };
	float GetHeight() const { return m_dimentions.m_y; };
	void SetObjectWidth(float newWidth) { m_dimentions.m_x = newWidth; };
	void SetObjectHeight(float newHeight) { m_dimentions.m_y = newHeight; };

	// Direction Related Functions //
	float GetRotationAngle() { return m_coordinates.m_rotation; };
	float GetXDirection() const { return m_direction.m_x; };
	float GetYDirection() const { return m_direction.m_y; };
	void SetXDirection(float newX) { m_direction.m_x = newX; };
	void SetYDirection(float newY) { m_direction.m_y = newY; };

	// Speed Related Functions //
	float GetSpeed() const { return m_speed; };
	void SetSpeed(float newSpeed) { m_speed = newSpeed; };

	// health related
	virtual float GetHealth() { return m_health; };
	virtual void SetHealth(float health) { m_health = health; };
	virtual float GetMaxHealth() { return m_maxHealth; };
	void ChangeHealth(float delta);
	virtual void CheckHealthLimit();

	// Object validation Related
	bool IsValid() const { return m_isValid; };
	void Invalidate();
	void Validate();

	// virtual 
	virtual void Init();
	virtual void Tick(float deltaTime);
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider);
	virtual void OverlapUpdate(ObjectBase* pOtherCollider);
	virtual void HandleEndOverlap(ObjectBase* pOtherCollider);
	virtual void Render(SDL_Renderer* pRenderer);
};

