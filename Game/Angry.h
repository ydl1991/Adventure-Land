#pragma once
#include "EnemyState.h"
class Enemy;

// TODO: Boss angry state
class Angry : public EnemyState
{
	float m_attackTriggerTime;										// Attack trigger time
	float m_timer;													// Timer for trigger attack
public:
	Angry(Enemy* pObject, float timer);
	~Angry();

	// Object State status
	virtual void EnterState() override;
	virtual void Update(float deltaTime) override;
	virtual void ExitState() override;

private:
	void ResetAttackTimer();										// Reset timer for next round of fireball
};

