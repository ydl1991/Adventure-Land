#include "Wander.h"
#include "Enemy.h"
#include "BaseState.h"

Wander::Wander(Enemy* pObject)
{
	m_pEnemyObject = pObject;
	m_type = BaseState::StateType::kWander;
}

Wander::~Wander()
{
	m_pEnemyObject = nullptr;
}

void Wander::EnterState()
{
	m_pEnemyObject->SetXDirection(0);
	m_pEnemyObject->SetYDirection(1);
}

void Wander::Update(float deltaTime)
{
	//
}

void Wander::ExitState()
{
	//
}
