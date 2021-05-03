#include "Angry.h"
#include "EnemyBoss.h"
#include "Enemy.h"
#include "ObjectBase.h"
#include "EventSystem.h"
Angry::Angry(Enemy* pObject, float timer)
	: m_attackTriggerTime{ timer }
	, m_timer{ 2.f }
{
	m_pEnemyObject = pObject;
	m_type = BaseState::StateType::kAngry;
}

Angry::~Angry()
{
	m_pEnemyObject = nullptr;
}

void Angry::EnterState()
{
	m_pEnemyObject->SetSpeed(160.f);
	m_pEnemyObject->SetObjectWidth(200.f);
	m_pEnemyObject->SetObjectHeight(200.f);
	m_pEnemyObject->GetCollisionSystem()->SetDimention(m_pEnemyObject->GetDimention());
}

void Angry::Update(float deltaTime)
{
	// check attack 
	if (m_pEnemyObject->GetHealth() > 0.f)
	{
		if (!m_pEnemyObject->IsAttacking())
		{
			m_timer -= deltaTime;
			// when ready to fire
			if (m_timer < 0)
			{
				// add attack event
				m_pEnemyObject->GetEventSystem()->AddNormalEventListener(m_pEnemyObject, Event::EventType::kAttact);

				// reset timer
				ResetAttackTimer();
			}
		}
	}
}

void Angry::ExitState()
{
}

void Angry::ResetAttackTimer()
{
	m_timer = m_attackTriggerTime;
}
