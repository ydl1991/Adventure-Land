#include "StateMachine.h"
#include "BaseState.h"

StateMachine::StateMachine()
	:m_pCurrentState{ nullptr }
{
	//
}

StateMachine::~StateMachine()
{
	//
	delete m_pCurrentState;
	m_pCurrentState = nullptr;
}

bool StateMachine::Init()
{
	return false;
}

void StateMachine::SetCurrentState(BaseState* newState)
{
	if (m_pCurrentState != nullptr)
	{
		m_pCurrentState->ExitState();
	}

	delete m_pCurrentState;
	m_pCurrentState = newState;

	if (m_pCurrentState != nullptr)
	{
		m_pCurrentState->EnterState();
	}
}

bool StateMachine::ProcessEvents()
{
	bool quit = false;

	if (m_pCurrentState != nullptr)
	{
		quit = m_pCurrentState->ProcessEvents();
	}

	return quit;
}

void StateMachine::Update(float deltaTime)
{
	if (m_pCurrentState != nullptr)
	{
		m_pCurrentState->Update(deltaTime);
	}
}

void StateMachine::RenderCurrentState(SDL_Renderer* pRenderer)
{
	if (m_pCurrentState != nullptr)
	{
		m_pCurrentState->RenderCurrentState(pRenderer);
	}
}
