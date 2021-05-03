#pragma once
#include "SDL_rect.h"
#include "ObjectBase.h"
struct SDL_Renderer;

//-----------------------------------------------------
//	Object that only plays animation and do not move
//-----------------------------------------------------
class AnimationObject : public ObjectBase
{
	// Trasform position of the object
	SDL_FRect m_animationTransform;
public:
	AnimationObject(float x, float y, float w, float h);
	~AnimationObject();

	// virtual function 
	virtual void Tick(float deltaTime) override;
	virtual void Render(SDL_Renderer* pRenderer) override;
};

