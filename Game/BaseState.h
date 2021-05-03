#pragma once
#include "SimpleSDLGame.h"
#include <stdint.h>

class BaseState
{
public:
	enum class StateType : uint8_t
	{
		kGameMenuState,
		kGameRunningState,
		kGameWinState,
		kGameLoseState,
		kWander,
		kAttack,
		kDeadFury,
		kAngry,
	};

protected:
	StateType m_type;							// A type variable to show state type
	SimpleSDLGame* m_pGameWorld;				// A reference of the Game world

public:
	BaseState() : m_pGameWorld{ nullptr }, m_type{} {};
	virtual ~BaseState() {};

	// Getter
	SimpleSDLGame* GetWorld() const { return m_pGameWorld; };
	StateType GetStateType() const { return m_type; }

	// Game State status
	virtual void EnterState() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void ExitState() = 0;
	// Function Overload, Event Processing
	virtual bool ProcessEvents() = 0;
	// Render
	virtual void RenderCurrentState(SDL_Renderer* pRenderer) = 0;
};