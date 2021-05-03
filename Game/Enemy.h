#pragma once
#include "ObjectBase.h"
#include "EventListener.h"
#include "SDL_rect.h"

class EventSystem;
class RandomNumberGenerator;
/***********************************************************/
// Enemy is a object that go against with player.
// it can collid with other object and make damage
/***********************************************************/
class Enemy : public ObjectBase, public EventListener
{
protected:
	bool m_isAttacking;							// Flag to check attacking
	bool m_isResting;							// Flag to check if currently stopped moving
	bool m_gotHit;								// Flag to check if got hit
	float m_restTimer;							// Determines how long does it stops moving
	EventSystem* m_pEventSystem;				// Event System to support event change
	RandomNumberGenerator* m_pLeftOrRightRNG;	// RNG that determines which direction to go

public:
	Enemy();
	~Enemy();

	// virtual override
	virtual void Init() override;
	virtual void Tick(float deltaTime) override;
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	virtual void Render(SDL_Renderer* pRenderer) override;

	// Getters
	bool IsAttacking() { return m_isAttacking; }
	EventSystem* GetEventSystem() const { return m_pEventSystem; }

	// Setters
	void SetGotHit(bool hit) { m_gotHit = hit; }
	void SetIsAttacking(bool attack) { m_isAttacking = attack; }

protected:
	virtual void GetHit();
	virtual void ResumeMoving();
	virtual void PositionUpdate(float deltaTime);
	virtual void ChangeDirection();
	virtual void ProcessEnemyEvent(Event::EventType evtType) override;
	virtual void ProcessObjectAttack();
	void CreateDeathParticleObject();

private:
	// Enemy action event
	void ProcessObjectMoveUp();
	void ProcessObjectMoveDown();
	void ProcessObjectMoveLeft();
	void ProcessObjectMoveRight();
	void ProcessObjectChasePlayer();
	void ProcessObjectGetHurt();
	void ProcessObjectHeal();
};

