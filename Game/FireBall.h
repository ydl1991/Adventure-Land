#pragma once
#include "ObjectBase.h"
class Vector2D;
class FireBall : public ObjectBase
{
public:
	// constructor and destructor //
	//FireBall(float x, float y, TextureSpriteSheet* pSpriteSheet, CollisionManager* pCManager, GameRunningState* pGameRunningState);
	FireBall(ObjectSpawnInfo objectSpawnInfo, Vector2D direction);
	~FireBall();

	// virtual override
	void Init() override;
	void Tick(float deltaTime) override;
	void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	void Render(SDL_Renderer* pRenderer) override;

private:
	void UpdatePosition(float deltaTime);
};

