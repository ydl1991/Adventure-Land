#pragma once
#include "Enemy.h"
class StateMachine;
//--------------------------------------------------------------//
//  Enemy with a dragon shape, can attack by throwing fire ball
//--------------------------------------------------------------//
class Dragon : public Enemy
{
	//float m_attackTimer;						// Timer for dragon to fire fireball
public:
	Dragon(ObjectSpawnInfo objectSpawnInfo);
	~Dragon();

	virtual void Tick(float deltaTime) override;		// Update loop
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;			// Handle overlap reaction
	virtual void Render(SDL_Renderer* pRenderer) override;									// Render current state of the object

private:
	virtual void PositionUpdate(float deltaTime) override;			// Update position in the map
	virtual void ChangeDirection() override;						// Update new directions in the map

	void CreateAndInitStateMachine();								// Create State Machine and Init it to Wander
	void ObjectStateSwitch();										// Check switch object state
};

