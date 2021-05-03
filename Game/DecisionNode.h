#pragma once
#include "DtNode.h"
class EnemyBoss;
class DecisionNode : public DtNode
{
public:
	virtual void ProcessNode(EnemyBoss* pBoss) override
	{
		if (Decide(pBoss))
		{
			if(m_pLeft)
				m_pLeft->ProcessNode(pBoss);
		}
		else
		{
			if(m_pRight)
				m_pRight->ProcessNode(pBoss);
		}
	}
	virtual bool Decide(EnemyBoss* pBoss) = 0;
};

