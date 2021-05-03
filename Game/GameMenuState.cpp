#include "GameMenuState.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "AnimationComponent.h"
#include "TextureSpriteSheet.h"
#include "GameRunningState.h"
#include "EventSystem.h"
#include "UIButton.h"
#include "ImageLoadingSystem.h"
#include "StateMachine.h"
#include "UITextField.h"
#include "AnimationObject.h"
#include "SimpleSDLGame.h"
#include "UIInputTaking.h"
#include "SoundPlayer.h"
#include "SoundLoadingSystem.h"
#include "UISavingLoading.h"
#include "ReadWriteSystem.h"
#include "UITextureButton.h"
#include <iostream>

//-----------------------------
//	Constructor
//-----------------------------
GameMenuState::GameMenuState(SimpleSDLGame* pGameWorld)
	: m_pTitle{nullptr}
	, m_pInstruction{nullptr}
	, m_pBackGround{ nullptr }
	, m_pSoundPlayer{ nullptr }
	, m_characterOneIsSelected{ false }
	, m_characterTwoIsSelected{ false }
	, m_pressQ{false}
	, m_pressCtrl{false}
	, m_pName{}
	, m_pSDLInputBox{ nullptr }
{
	m_pGameWorld = pGameWorld;
	//m_pSavingLoadingMenu = pGameWorld->GetSavingLoadingMenu();

	m_type = BaseState::StateType::kGameMenuState;

	m_pCharacterOneButton = nullptr;
	m_pCharacterTwoButton = nullptr;

	// load ttf and animation objects and buttons
	LoadText();
	LoadAllObjectsRequiresAnimation();
	LoadBackgroundSounds();

	// Add the object as Event Listener
	m_pGameWorld->GetEventSystem()->AddInputEventListener(this);
}

//-----------------------------
//	Destructor
//-----------------------------
GameMenuState::~GameMenuState()
{
	// Remove the object as Event Listener
	m_pGameWorld->GetEventSystem()->RemoveInputEventListener(this);

	// clear text texture
	delete m_pTitle;
	m_pTitle = nullptr;

	delete m_pInstruction;
	m_pInstruction = nullptr;

	// clear background and button memory
	delete m_pBackGround;
	m_pBackGround = nullptr;

	delete m_pCharacterOneButton;
	m_pCharacterOneButton = nullptr;

	delete m_pCharacterTwoButton;
	m_pCharacterTwoButton = nullptr;

	delete m_pSaveLoadMenuButton;
	m_pSaveLoadMenuButton = nullptr;

	delete m_pSDLInputBox;
	m_pSDLInputBox = nullptr;

	delete m_pSoundPlayer;
	m_pSoundPlayer = nullptr;

	m_pGameWorld = nullptr;
}

//-----------------------------
//	Init Game State
//-----------------------------
void GameMenuState::EnterState()
{
	// set button one callback
	m_pCharacterOneButton->SetCallback([this]
	{
		GameState* pState = new GameRunningState(m_pGameWorld, 0, m_pName.c_str());
		m_pGameWorld->GetStateMachine()->SetCurrentState(pState);
		pState = nullptr;
	});

	// set button two callback
	m_pCharacterTwoButton->SetCallback([this]
	{
		GameState* pState = new GameRunningState(m_pGameWorld, 1, m_pName.c_str());
		m_pGameWorld->GetStateMachine()->SetCurrentState(pState);
		pState = nullptr;
	});

	m_pSaveLoadMenuButton->SetCallback([this]() 
	{
		m_pGameWorld->GetSavingLoadingMenu()->SetCurrentMenu(UISavingLoading::Menu::kEntry);
	});

	// Background anim
	m_pBackGround->GetAnimationComponent()->AddAnimationSequence("BG", 0, 7);
	m_pBackGround->GetAnimationComponent()->PlayAnimation("BG");

	// human character
	m_pCharacterOneButton->SetAnimationNotHighlighted(0, 0);
	m_pCharacterOneButton->SetAnimationHighlighted(7, 17);


	// warplan character
	m_pCharacterTwoButton->SetAnimationNotHighlighted(12, 12);
	m_pCharacterTwoButton->SetAnimationHighlighted(12, 14);

	// menu button anim
	m_pSaveLoadMenuButton->SetAnimationNotHighlighted(0, 0);
	m_pSaveLoadMenuButton->SetAnimationHighlighted(1, 1);
}

//-----------------------------
//	Update Current Game State
//-----------------------------
void GameMenuState::Update(float deltaTime)
{
	// update animation background
	m_pBackGround->Tick(deltaTime);

	// update animation buttons
	m_pCharacterOneButton->Update(deltaTime);
	m_pCharacterTwoButton->Update(deltaTime);
	m_pSaveLoadMenuButton->Update(deltaTime);
	m_pGameWorld->GetSavingLoadingMenu()->Update(deltaTime);

	// update button postion
	if (m_pSDLInputBox == nullptr && (m_characterOneIsSelected || m_characterTwoIsSelected))
	{
		CreateSDLInputTakingBox();
	}

	if (m_pSDLInputBox && !m_pSDLInputBox->Quit() && !m_pSDLInputBox->Done())
	{
		m_pSDLInputBox->Update(deltaTime);
	}

	if (m_pSDLInputBox && m_pSDLInputBox->Quit())
	{
		m_characterOneIsSelected = false;
		m_characterTwoIsSelected = false;
		delete m_pSDLInputBox;
		m_pSDLInputBox = nullptr;
	}
	else if (m_pSDLInputBox && m_pSDLInputBox->Done())
	{	
		Mix_FadeOutMusic(3000);
		m_pName = m_pSDLInputBox->GetInputTextString();

		if (m_characterOneIsSelected)
		{
			//Mix_HookMusicFinished(&m_pCharacterOneButton->Trigger);
			m_pCharacterOneButton->ButtonMoving(0, -0.05f);
		}
		else if (m_characterTwoIsSelected)
		{
			//Mix_HookMusicFinished(&m_pCharacterTwoButton->Trigger);
			m_pCharacterTwoButton->ButtonMoving(0, -0.05f);
		}
	}

	// trigger button click callback condition
	if (m_pCharacterOneButton->GetTransform().y < -m_pCharacterOneButton->GetTransform().h)
	{
		m_pCharacterOneButton->Trigger();
		return;
	}

	if (m_pCharacterTwoButton->GetTransform().y < -m_pCharacterTwoButton->GetTransform().h)
	{
		m_pCharacterTwoButton->Trigger();
		return;
	}
}

//-----------------------------
//	Exit Game State
//-----------------------------
void GameMenuState::ExitState()
{
}

//-----------------------------
//	Connect Event System 
//	Process Game State Events
//-----------------------------
bool GameMenuState::ProcessEvents()
{
	m_pGameWorld->GetEventSystem()->ProcessSDLInputEvents();

	return false;
}

void GameMenuState::ProcessInputEvent(SDL_Event& evt)
{
	switch (evt.type)
	{
		// keyboard events
	case SDL_KEYDOWN:
	{
		if (GetSelectionState())
			ProcessInputBoxKeyboardEvent(&evt.key);
		else
			ProcessMenuKeyboardEvent(&evt.key);
	}
	break;
	case SDL_TEXTINPUT:
		ProcessInputTextEvent(m_pSDLInputBox->GetInputTextString(), &evt.text);
		break;
		// mouse events
	case SDL_MOUSEBUTTONDOWN:
		ProcessMenuClickEvent(&evt.button);
		break;
	case SDL_MOUSEMOTION:
		ProcessMenuMouseMotionEvent(&evt.motion);
		break;
		// window events
	case SDL_WINDOWEVENT:
		ProcessWindowEvent(&evt.window);
		break;
	}
}

//-----------------------------
//	Render Current State
//-----------------------------
void GameMenuState::RenderCurrentState(SDL_Renderer* pRenderer)
{
	SDL_RenderClear(pRenderer);
	// render background
	m_pBackGround->Render(pRenderer);

	// render text
	m_pTitle->Render(pRenderer);
	m_pInstruction->Render(pRenderer);

	// render character one
	m_pCharacterOneButton->Render(pRenderer);
	// render character two
	m_pCharacterTwoButton->Render(pRenderer);
	// render saving loading menu button
	m_pSaveLoadMenuButton->Render(pRenderer);

	// render input box
	if(m_pSDLInputBox && !m_pSDLInputBox->Quit() && !m_pSDLInputBox->Done())
		m_pSDLInputBox->Render(pRenderer);

	m_pGameWorld->GetSavingLoadingMenu()->Render(pRenderer);

	// present on screen
	SDL_RenderPresent(pRenderer);
}

//-----------------------------
//	UIText Loading
//-----------------------------
void GameMenuState::LoadText()
{
	//this opens a font style and sets a size
	TTF_Font* pTitleFont = TTF_OpenFont("Fonts/Satisfy-Regular.ttf", 80);
	if (!pTitleFont)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}
	// set font style
	TTF_SetFontStyle(pTitleFont, TTF_STYLE_BOLD);
	// this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_Color orange = { 255, 153, 0, 255 };
	// create UI Text
	m_pTitle = new UITextField(45, 50, pTitleFont, "ADVANTURE LAND", orange, m_pGameWorld->GetRenderer());

	//this opens a font style and sets a size
	TTF_Font* pInstructionFont = TTF_OpenFont("Fonts/Teko-Bold.ttf", 45);
	if (!pInstructionFont)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}
	// set font style
	TTF_SetFontStyle(pInstructionFont, TTF_STYLE_ITALIC);
	// this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_Color blue = { 0, 172, 230, 255 };  
	// create UI Text
	m_pInstruction = new UITextField(153, 200, pInstructionFont, "CHOOSE   YOUR   CHARACTER", blue, m_pGameWorld->GetRenderer());
}

//--------------------------------------
//	Animation Object and Button Loading
//--------------------------------------
void GameMenuState::LoadAllObjectsRequiresAnimation()
{
	// texture variable
	TextureSpriteSheet* pBackground = nullptr;
	TextureSpriteSheet* pPlayerOne = nullptr;
	TextureSpriteSheet* pPlayerTwo = nullptr;
	TextureSpriteSheet* pSaveMenu = nullptr;

	// get renderer
	SDL_Renderer* pRenderer = GetWorld()->GetRenderer();

	// load sprite sheet
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/mainMenu1.png", pBackground);
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/CharacterAnimation/animation1.png", pPlayerOne);
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/CharacterAnimation/warplane1.png", pPlayerTwo);
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/Button/saveMenu.png", pSaveMenu);

	// background object
	m_pBackGround = new AnimationObject(0,0,0,0);
	m_pBackGround->AddAnimationComponent(new AnimationComponent(pBackground, 12.f, 700, 400, 8));

	// Animation player character 1
	m_pCharacterOneButton = new UIButton(250.f, 267.f, 100.f, 120.f, new AnimationComponent(pPlayerOne, 12.f, 80, 100, 18));

	// Animation player character 2
	m_pCharacterTwoButton = new UIButton(450.f, 280.f, 100.f, 120.f, new AnimationComponent(pPlayerTwo, 6.f, 64, 64, 16));

	// Save menu button
	m_pSaveLoadMenuButton = new UIButton(20, 540, 70, 35, new AnimationComponent(pSaveMenu, 1.f, 400, 251, 2));
}

//--------------------------------------
//	Create a input message box
//--------------------------------------
void GameMenuState::CreateSDLInputTakingBox()
{
	SDL_Texture* pTexture = nullptr;

	SDL_Renderer* pRenderer = GetWorld()->GetRenderer();

	m_pGameWorld->GetImageLoadingSystem()->Load(pRenderer, "Assets/inputBox.png", pTexture);

	m_pSDLInputBox = new UIInputTaking(m_pGameWorld, "Enter Your Name", pTexture);
	m_pSDLInputBox->SetBoarderRect(200, 200, 400, 200);
	m_pSDLInputBox->SetTextRect(300, 300, 200, 40);
}

////--------------------------------------
////	Create UI Saving Loading Menu
////--------------------------------------
//void GameMenuState::CreateUISavingLoadingMenu()
//{
//	m_pSavingLoadingMenu = new UISavingLoading(m_pGameWorld);
//}

//--------------------------------------
//	Create Background Sound
//--------------------------------------
void GameMenuState::LoadBackgroundSounds()
{
	m_pSoundPlayer = new SoundPlayer();
	Mix_Music* pBackgroundMusic = nullptr;
	m_pGameWorld->GetSoundLoadingSystem()->Load("Sounds/Music/MenuBackgroundSound.mp3", pBackgroundMusic);
	m_pSoundPlayer->SetSoundTrack(pBackgroundMusic);
	m_pSoundPlayer->PlayMusic(true, -1, 2000);
}

void GameMenuState::ProcessMenuClickEvent(SDL_MouseButtonEvent* pData)
{
	UIButton* pButtonOne = m_pCharacterOneButton;
	UIButton* pButtonTwo = m_pCharacterTwoButton;
	UIButton* pButtonSaveMenu = m_pSaveLoadMenuButton;
	ReadWriteSystem* pReadWriteSystem = m_pGameWorld->GetReadWriteSystem();

	switch (pData->button)
	{
		// motion moving condition
	case SDL_BUTTON_LEFT:
	{
		UISavingLoading* pMenu = m_pGameWorld->GetSavingLoadingMenu();
		// if save and load menu is not open
		if (pMenu && pMenu->GetCurrentMenu() == UISavingLoading::Menu::kOff)
		{
			if (!GetSelectionState())
			{
				if (pButtonOne->HitTest(pData->x, pData->y))
				{
					SelectCharacterOne();
				}

				if (pButtonTwo->HitTest(pData->x, pData->y))
				{
					SelectCharacterTwo();
				}

				if (pButtonSaveMenu->HitTest(pData->x, pData->y))
				{
					pButtonSaveMenu->Trigger();
				}
			}
		}
		// if open save and load menu and at entry state
		else if (pMenu && pMenu->GetCurrentMenu() == UISavingLoading::Menu::kEntry)
		{
			if (pReadWriteSystem->GetNumOfFileSaved() > 0 && pMenu->GetLoadButton()->HitTest(pData->x, pData->y))
			{
				pMenu->GetLoadButton()->Trigger();
				return;
			}
		}
		// if opened load menu and clicked, trigger load file
		else if (pMenu && pMenu->GetCurrentMenu() == UISavingLoading::Menu::kLoad)
		{
			if (pReadWriteSystem->GetFileOne().m_isOccupied && pMenu->GetGameOneSlot()->HitTest(pData->x, pData->y))
			{
				pMenu->GetGameOneSlot()->Trigger();
				return;
			}

			if (pReadWriteSystem->GetFileTwo().m_isOccupied && pMenu->GetGameTwoSlot()->HitTest(pData->x, pData->y))
			{
				pMenu->GetGameTwoSlot()->Trigger();
				return;
			}

			if (pReadWriteSystem->GetFileThree().m_isOccupied && pMenu->GetGameThreeSlot()->HitTest(pData->x, pData->y))
			{
				pMenu->GetGameThreeSlot()->Trigger();
				return;
			}
		}
	}
	break;
	default:
		break;
	}

}

void GameMenuState::ProcessMenuMouseMotionEvent(SDL_MouseMotionEvent* pData)
{
	UIButton* pButtonOne = m_pCharacterOneButton;
	UIButton* pButtonTwo = m_pCharacterTwoButton;
	UIButton* pButtonSaveMenu = m_pSaveLoadMenuButton;

	if (!GetSelectionState())
	{
		if (pButtonOne->HitTest(pData->x, pData->y))
		{
			pButtonOne->SetIsHighlighted(true);
		}
		else
			pButtonOne->SetIsHighlighted(false);

		if (pButtonTwo->HitTest(pData->x, pData->y))
		{
			pButtonTwo->SetIsHighlighted(true);
		}
		else
			pButtonTwo->SetIsHighlighted(false);

		if (pButtonSaveMenu->HitTest(pData->x, pData->y))
		{
			pButtonSaveMenu->SetIsHighlighted(true);
		}
		else
			pButtonSaveMenu->SetIsHighlighted(false);
	}
}

void GameMenuState::ProcessMenuKeyboardEvent(SDL_KeyboardEvent* pData)
{
	//Log key name
	const char* keyName = SDL_GetKeyName(pData->keysym.sym);
	if (pData->repeat)
		return;

	switch (pData->keysym.scancode)
	{
		// case to open save and load menu
	case SDL_SCANCODE_ESCAPE:
	{
		UISavingLoading* pMenu = m_pGameWorld->GetSavingLoadingMenu();
		if (pMenu && pMenu->GetCurrentMenu() != UISavingLoading::Menu::kOff)
			pMenu->SetCurrentMenu(UISavingLoading::Menu::kOff);
	}
	break;
	default:
		break;
	}
}

void GameMenuState::ProcessWindowEvent(SDL_WindowEvent* pData)
{
	switch (pData->event)
	{
		// click the "x" on upper-right of the window to close window
	case SDL_WINDOWEVENT_CLOSE:
		std::cout << "Window Closing!" << std::endl;
		m_pGameWorld->ShutDown();
		break;
	default:
		break;
	}
}

void GameMenuState::ProcessInputBoxKeyboardEvent(SDL_KeyboardEvent* pData)
{
	std::string& text = m_pSDLInputBox->GetInputTextString();

	if (pData->keysym.sym == SDLK_BACKSPACE && text.length() > 1)
	{
		text = text.substr(0, text.length() - 1);
	}
	else if (pData->keysym.sym == SDLK_RETURN || pData->keysym.sym == SDLK_KP_ENTER)
	{
		SDL_StopTextInput();
		m_pSDLInputBox->SetDone(true);
	}
	else if (pData->keysym.sym == SDLK_ESCAPE)
	{
		SDL_StopTextInput();
		m_pSDLInputBox->SetQuit(true);
	}
}

void GameMenuState::ProcessInputTextEvent(std::string& text, SDL_TextInputEvent* pData)
{
	std::cout << "" << pData->text << std::endl;
	if (text.length() < 10)
		text += pData->text;
}
