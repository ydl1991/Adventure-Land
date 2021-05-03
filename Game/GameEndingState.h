#pragma once
#include "GameState.h"
#include "EventListener.h"
#include <string>
class UITextField;
class UIButton;
class GameEndingState : public GameState, public EventListener
{
protected:
	UITextField* m_pGameMakerCredit;			// Show message to give credit	
	UIButton* m_pMainMenuButton;				// Go to main menu
	UIButton* m_pReplayButton;					// Replay again
	UIButton* m_pQuitButton;					// Quit the game
	int m_playerType;							// keep a player type ID here for potential play again
	std::string m_pName;						// a copy of the player's name for replay

public:
	GameEndingState();
	virtual ~GameEndingState();

	// Game State status
	virtual void EnterState();
	virtual void Update(float deltaTime) {};
	virtual void ExitState() {};

	// Render
	virtual void RenderCurrentState(SDL_Renderer* pRenderer) {};

	// Getters
	UIButton* GetMenuButton() const { return m_pMainMenuButton; }
	UIButton* GetReplayButton() const { return m_pReplayButton; }
	UIButton* GetQuitButton() const { return m_pQuitButton; }
};

