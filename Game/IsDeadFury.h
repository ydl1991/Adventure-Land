#pragma once
#include "DecisionNode.h"
#include "EnemyBoss.h"
#include "ObjectState.h"
#include "StateMachine.h"
class IsDeadFury : public DecisionNode
{
public:
	virtual bool Decide(EnemyBoss* pBoss) override
	{
		if (pBoss->GetObjectStateMachine()->GetCurrentState()->GetStateType() == BaseState::StateType::kDeadFury)
		{
			return true;
		}

		return false;
	}
};

