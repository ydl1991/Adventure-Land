#pragma once
#include "DtNode.h"
#include "EnemyBoss.h"
class DeadlyAttack : public DtNode
{
public:
	virtual void ProcessNode(EnemyBoss* pBoss) override
	{
		pBoss->DeadlyAbilityAttack();
	}
};

