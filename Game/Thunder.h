#pragma once
#include "ObjectBase.h"
class EnemyBoss;

class Thunder : public ObjectBase
{
	EnemyBoss* m_pOwner;
	bool m_isActive;

public:
	// constructor and destructor //
	Thunder(EnemyBoss* pBoss, TextureSpriteSheet* pSpriteSheet, CollisionManager* pCManager, GameRunningState* pGameRunningState);
	~Thunder();

	// virtual override
	void Init() override;
	void Tick(float deltaTime) override;
	void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	void Render(SDL_Renderer* pRenderer) override;

	// getter setters
	bool GetActiveStatus() const { return m_isActive; }
	void SetActiveStatus(bool active) { m_isActive = active; }

private:
	void UpdatePosition(float deltaTime);
};

