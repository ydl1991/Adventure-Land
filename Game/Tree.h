#pragma once
#include "Enemy.h"
class Tree :
	public Enemy
{
public:
	Tree(ObjectSpawnInfo objectSpawnInfo);
	~Tree();

	virtual void Tick(float deltaTime) override;
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	virtual void Render(SDL_Renderer* pRenderer) override;
private:
	virtual void PositionUpdate(float deltaTime) override;
	virtual void ChangeDirection() override;

	void CreateAndInitStateMachine();								// Create State Machine and Init it to Wander
	void ObjectStateSwitch();										// Check switch object state
};

