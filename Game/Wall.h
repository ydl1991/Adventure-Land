#pragma once
#include "ObjectBase.h"

class GameRunningState;
struct SDL_Texture;

/***********************************************************/
// A static object that is unbreakable and cannot pass through
/***********************************************************/
class Wall : public ObjectBase
{
protected:
	Wall(){};
public:
	Wall(ObjectSpawnInfo objectSpawnInfo);
	~Wall();

	// virtual override
	virtual void Init() override;
	virtual void Tick(float deltaTime) override;
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	virtual void Render(SDL_Renderer* pRenderer) override;

private:
	void CreateCrackedParticle();
};

