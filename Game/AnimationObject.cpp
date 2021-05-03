
#include "AnimationObject.h"

AnimationObject::AnimationObject(float x, float y, float w, float h)
	:m_animationTransform{x,y,w,h}
{
}

AnimationObject::~AnimationObject()
{
	//
}

void AnimationObject::Tick(float deltaTime)
{
	m_pAnimationComponent->Update(deltaTime);
}

void AnimationObject::Render(SDL_Renderer* pRenderer)
{
	m_pAnimationComponent->Render(pRenderer);
}
