#pragma once
#include "DecisionNode.h"
#include "EnemyBoss.h"
#include "ObjectState.h"
#include "StateMachine.h"
class IsAngry : public DecisionNode
{
public:
	virtual bool Decide(EnemyBoss* pBoss) override
	{
		if (pBoss->GetObjectStateMachine()->GetCurrentState()->GetStateType() == BaseState::StateType::kAngry)
		{
			return true;
		}

		return false;
	}
};

