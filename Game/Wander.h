#pragma once
#include "EnemyState.h"
class Enemy;
class Wander : public EnemyState
{
public:
	Wander(Enemy* pEnemyObject);
	~Wander();

	// Object State status
	virtual void EnterState() override;
	virtual void Update(float deltaTime) override;
	virtual void ExitState() override;
};

