#pragma once
#include "EnemyState.h"
class Enemy;
class Attack : public EnemyState
{
	float m_attackTriggerTime;										// Attack trigger time
	float m_timer;													// Timer for trigger attack
public:
	Attack(Enemy* pObject, float timer);
	~Attack();

	// Object State status
	virtual void EnterState() override;
	virtual void Update(float deltaTime) override;
	virtual void ExitState() override;

private:
	void ResetAttackTimer();										// Reset timer for next round of fireball
	void ChaseAttack();												// Action under kAttack state
};