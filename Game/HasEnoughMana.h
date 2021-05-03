#pragma once
#include "DecisionNode.h"
#include "EnemyBoss.h"

class HasEnoughMana : public DecisionNode
{
public:
	virtual bool Decide(EnemyBoss* pBoss) override
	{
		if (pBoss->GetMana() > 50.f)
		{
			return true;
		}

		return false;
	}
};

