#pragma once
#include "ObjectBase.h"
#include "SDL_rect.h"

/***********************************************************/
// Projectile is a weapon object that can be fired by player
// or enemy. It can collid with other object and make damage
/***********************************************************/
class Projectile : public ObjectBase
{
public:
	// constructor and destructor //
	Projectile() {}
	Projectile(ObjectSpawnInfo weaponInfo);
	~Projectile();

	// virtual override
	void Init() override;
	void Tick(float deltaTime) override;
	void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	void Render(SDL_Renderer* pRenderer) override;

protected:
	void UpdatePosition(float deltaTime);
};

