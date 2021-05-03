#include "GameEndingState.h"
#include "EventSystem.h"
#include "UITextField.h"
#include "UIButton.h"


GameEndingState::GameEndingState()
	: m_pGameMakerCredit{ nullptr }
	, m_pMainMenuButton{nullptr}
	, m_pReplayButton{nullptr}
	, m_pQuitButton{nullptr}
	, m_playerType{0}
	, m_pName{""}
{
	//
}

GameEndingState::~GameEndingState()
{
	delete m_pGameMakerCredit;
	m_pGameMakerCredit = nullptr;

	delete m_pMainMenuButton;
	m_pMainMenuButton = nullptr;

	delete m_pReplayButton;
	m_pReplayButton = nullptr;

	delete m_pQuitButton;
	m_pQuitButton = nullptr;

	m_pGameWorld = nullptr;
}

void GameEndingState::EnterState()
{
	// Reset Event Transition
	m_pGameWorld->GetEventSystem()->SetTransition(false);
}
