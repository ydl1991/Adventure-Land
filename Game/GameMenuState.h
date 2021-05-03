#pragma once
#include "GameState.h"
#include "EventListener.h"
#include <string>
struct SDL_Texture;
struct SDL_Renderer;
struct TextureSpriteSheet;
class AnimationComponent;
class SimpleSDLGame;
class AnimationObject;
class UIButton;
class UITextField;
class UIInputTaking;
class SoundPlayer;
class UISavingLoading;

class GameMenuState : public GameState, public EventListener
{
	// Player name entered
	std::string m_pName;

	// Animation Background
	AnimationObject* m_pBackGround;

	// Sound Player
	SoundPlayer* m_pSoundPlayer;

	// Button Object
	UIButton* m_pCharacterOneButton;
	UIButton* m_pCharacterTwoButton;
	UIButton* m_pSaveLoadMenuButton;

	// Text fields
	UITextField* m_pTitle;
	UITextField* m_pInstruction;

	// Input Taking UI
	UIInputTaking* m_pSDLInputBox;									

	// select state
	bool m_characterOneIsSelected;
	bool m_characterTwoIsSelected;

	// Quit Game Conditions
	bool m_pressCtrl;
	bool m_pressQ;

public:
	GameMenuState(SimpleSDLGame* pGameWorld);
	~GameMenuState();

	// Animation system related
	UIButton* GetButtonOne() const { return m_pCharacterOneButton; };
	UIButton* GetButtonTwo() const { return m_pCharacterTwoButton; };
	UIButton* GetSaveLoadMenuButton() const { return m_pSaveLoadMenuButton; }

	// Game State status
	virtual void EnterState() override;
	virtual void Update(float deltaTime) override;
	virtual void ExitState() override;
	// Function Overload, Event Processing
	virtual bool ProcessEvents() override;
	void ProcessInputEvent(SDL_Event& evt) override;								// Process Input Event
	// Render
	virtual void RenderCurrentState(SDL_Renderer* pRenderer) override;

	// Setter
	void SelectCharacterOne() { m_characterOneIsSelected = true; };
	void SelectCharacterTwo() { m_characterTwoIsSelected = true; };

	// Getters
	bool GetSelectionState() const { return (m_characterOneIsSelected || m_characterTwoIsSelected); };
	UIInputTaking* GetInputBox() { return m_pSDLInputBox; };

private:
	void LoadText();
	void LoadAllObjectsRequiresAnimation();
	void CreateSDLInputTakingBox();
	/*void CreateUISavingLoadingMenu();*/
	void LoadBackgroundSounds();

	// Game State Action Event
	void ProcessMenuClickEvent(SDL_MouseButtonEvent* pData);
	void ProcessMenuMouseMotionEvent(SDL_MouseMotionEvent* pData);
	void ProcessMenuKeyboardEvent(SDL_KeyboardEvent* pData);
	void ProcessWindowEvent(SDL_WindowEvent* pData);
	/*******************************************************/
	//					SDL Input Text Events
	/*******************************************************/
	void ProcessInputBoxKeyboardEvent(SDL_KeyboardEvent* pData);
	/*******************************************************/
	//					SDL Input Text Event Related
	/*******************************************************/
	void ProcessInputTextEvent(std::string& text, SDL_TextInputEvent* pData);
};

