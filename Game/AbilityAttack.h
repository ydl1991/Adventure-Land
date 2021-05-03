#pragma once
#include "DtNode.h"
#include "EnemyBoss.h"
class AbilityAttack : public DtNode
{
public:
	virtual void ProcessNode(EnemyBoss* pBoss) override
	{
		pBoss->AttackAbility();
	}
};

