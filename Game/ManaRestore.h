#pragma once
#include "DtNode.h"
#include "EnemyBoss.h"
class ManaRestore : public DtNode
{
public:
	virtual void ProcessNode(EnemyBoss* pBoss) override
	{
		pBoss->ManaRestoreAbility();
	}
};

