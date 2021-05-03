#pragma once
#include "ObjectState.h"
class Enemy;
class EnemyState : public ObjectState
{
protected:
	Enemy* m_pEnemyObject;
public:
	EnemyState() : m_pEnemyObject{nullptr} {};
	virtual ~EnemyState() {};

	// Getter
	Enemy* GetObject() const { return m_pEnemyObject; }
};

