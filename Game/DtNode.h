#pragma once

class EnemyBoss;

class DtNode
{
public:
	DtNode() :m_pLeft{ nullptr }, m_pRight{nullptr}{}
	virtual ~DtNode()
	{
		delete m_pLeft;
		m_pLeft = nullptr;

		delete m_pRight;
		m_pRight = nullptr;
	}

	virtual void ProcessNode(EnemyBoss* pBoss) = 0;

	void SetRightChild(DtNode* right) { m_pRight = right; }
	void SetLeftChild(DtNode* left) { m_pLeft = left; }

protected:
	DtNode* m_pLeft;
	DtNode* m_pRight;
};

