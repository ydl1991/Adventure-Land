#pragma once
#include <SDL.h>

class BaseState;

/***********************************************************/
// State Machine that processes game states
/***********************************************************/
class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	bool Init();

	void SetCurrentState(BaseState* newState);
	bool ProcessEvents();
	void Update(float deltaTime);
	void RenderCurrentState(SDL_Renderer* pRenderer);
	BaseState* GetCurrentState() const { return m_pCurrentState; }
private:
	BaseState* m_pCurrentState;
};

