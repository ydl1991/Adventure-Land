#include "Attack.h"
#include "EventSystem.h"
#include "BaseState.h"
#include "Enemy.h"

Attack::Attack(Enemy* pObject, float timer)
	: m_attackTriggerTime{ timer }
	, m_timer{0}
{
	m_pEnemyObject = pObject;
	m_type = BaseState::StateType::kAttack;
}

Attack::~Attack()
{
	m_pEnemyObject = nullptr;
}

void Attack::EnterState()
{
	//
}

void Attack::Update(float deltaTime)
{
	// check attack and retarget chase direction
	m_timer -= deltaTime;
	ChaseAttack();
}

void Attack::ExitState()
{
	//
}

void Attack::ResetAttackTimer()
{
	m_timer = m_attackTriggerTime;
}

void Attack::ChaseAttack()
{
	// when ready to fire
	if (m_timer < 0)
	{
		// add attack event
		m_pEnemyObject->GetEventSystem()->AddNormalEventListener(m_pEnemyObject, Event::EventType::kAttact);
	
		// reset timer
		ResetAttackTimer();
	}

	// call chase event to move towards player
	m_pEnemyObject->GetEventSystem()->AddNormalEventListener(m_pEnemyObject, Event::EventType::kChase);
}
