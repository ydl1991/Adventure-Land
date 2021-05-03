#include "GameWinState.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_render.h"
#include "AnimationComponent.h"
#include "TextureSpriteSheet.h"
#include "StateMachine.h"
#include "UITextField.h"
#include "AnimationObject.h"
#include "GameRunningState.h"
#include "UIButton.h"
#include "GameMenuState.h"
#include "EventSystem.h"
#include "ImageLoadingSystem.h"
#include "SimpleSDLGame.h"
#include "SoundLoadingSystem.h"
#include "SoundPlayer.h"
#include <iostream>

GameWinState::GameWinState(SimpleSDLGame* pGameWorld, int playerType, std::string pName)
{
	m_pGameWorld = pGameWorld;
	m_type = BaseState::StateType::kGameWinState;
	m_playerType = playerType;
	m_pName = pName;
	LoadText();
	LoadAnimations();
	LoadBackgroundSounds();

	// Load Player Credit
	TTF_Font* pFont = TTF_OpenFont("Fonts/Hollows.ttf", 20);
	if (!pFont)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
	}
	// this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_Color white = { 255, 255, 255, 255 };
	m_pGameMakerCredit = new UITextField(230, 570, pFont, "Game Owned And Written By Dali Yi", white, pGameWorld->GetRenderer());

	// Add the object as Event Listener
	m_pGameWorld->GetEventSystem()->AddInputEventListener(this);
}

GameWinState::~GameWinState()
{
	// Remove the object as Event Listener
	m_pGameWorld->GetEventSystem()->RemoveInputEventListener(this);

	delete m_pAnimBackground;
	m_pAnimBackground = nullptr;

	delete m_pMessageOne;
	m_pMessageOne = nullptr;

	delete m_pSoundPlayer;
	m_pSoundPlayer = nullptr;

	m_pGameWorld = nullptr;
}

void GameWinState::EnterState()
{
	GameEndingState::EnterState();

	// Background anim
	m_pAnimBackground->GetAnimationComponent()->AddAnimationSequence("BG", 0, 15);
	m_pAnimBackground->GetAnimationComponent()->PlayAnimation("BG");

	// Set Button anim
	m_pMainMenuButton->SetAnimationNotHighlighted(0, 0);
	m_pReplayButton->SetAnimationNotHighlighted(0, 0);
	m_pQuitButton->SetAnimationNotHighlighted(0, 0);

	m_pMainMenuButton->SetAnimationHighlighted(1, 1);
	m_pReplayButton->SetAnimationHighlighted(1, 1);
	m_pQuitButton->SetAnimationHighlighted(1, 1);

	// Set Button Callback
	m_pMainMenuButton->SetCallback([this] {
		GameState* pState = new GameMenuState(m_pGameWorld);
		m_pGameWorld->GetStateMachine()->SetCurrentState(pState);
		pState = nullptr;
	});

	m_pReplayButton->SetCallback([this] {
		GameState* pState = new GameRunningState(m_pGameWorld, m_playerType, m_pName);
		m_pGameWorld->GetStateMachine()->SetCurrentState(pState);
		pState = nullptr;
	});

	m_pQuitButton->SetCallback([this] {
		m_pGameWorld->ShutDown();
	});
}

void GameWinState::Update(float deltaTime)
{
	m_pAnimBackground->Tick(deltaTime);

	m_pMainMenuButton->Update(deltaTime);
	m_pReplayButton->Update(deltaTime);
	m_pQuitButton->Update(deltaTime);
}

void GameWinState::ExitState()
{
}

bool GameWinState::ProcessEvents()
{
	m_pGameWorld->GetEventSystem()->ProcessSDLPostGameInputEvents();

	return false;
}

void GameWinState::ProcessInputEvent(SDL_Event& evt)
{
	switch (evt.type)
	{
		// mouse events
	case SDL_MOUSEBUTTONDOWN:
		ProcessEndClickEvent(&evt.button);
		break;
	case SDL_MOUSEMOTION:
		ProcessEndMouseMotionEvent(&evt.motion);
		break;
		// window events
	case SDL_WINDOWEVENT:
		ProcessWindowEvent(&evt.window);
		break;
	default:
		break;
	}
}

void GameWinState::RenderCurrentState(SDL_Renderer* pRenderer)
{
	SDL_RenderClear(pRenderer);
	// render background
	m_pAnimBackground->Render(pRenderer);
	// render buttons
	m_pMainMenuButton->Render(pRenderer);
	m_pReplayButton->Render(pRenderer);
	m_pQuitButton->Render(pRenderer);
	// render text
	m_pMessageOne->Render(pRenderer);
	m_pGameMakerCredit->Render(pRenderer);
	// present on screen
	SDL_RenderPresent(pRenderer);
}

void GameWinState::LoadText()
{
	//this opens a font style and sets a size
	TTF_Font* pFont = TTF_OpenFont("Fonts/Satisfy-Regular.ttf", 35);
	if (!pFont)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
	}
	// set font style
	TTF_SetFontStyle(pFont, TTF_STYLE_BOLD);
	// this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_Color gold = { 230, 230, 0, 255 };

	m_pMessageOne = new UITextField(85, 200, pFont, "WINNER WINNER CHICKEN DINNER", gold, m_pGameWorld->GetRenderer());
}

void GameWinState::LoadAnimations()
{
	// texture variable
	TextureSpriteSheet* pBackground = nullptr;
	TextureSpriteSheet* pMenuButton = nullptr;
	TextureSpriteSheet* pReplayButton = nullptr;
	TextureSpriteSheet* pQuitButton = nullptr;

	// get renderer
	SDL_Renderer* pRenderer = GetWorld()->GetRenderer();

	// load sprite sheet
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/win.png", pBackground);
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/Button/Menu-green.png", pMenuButton);
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/Button/Replay-green.png", pReplayButton);
	GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/Button/Quit-green.png", pQuitButton);

	m_pAnimBackground = new AnimationObject(0,0,0,0);
	m_pAnimBackground->AddAnimationComponent(new AnimationComponent(pBackground, 12.f, 640, 490, 16));

	m_pMainMenuButton = new UIButton(20, 460, 70, 35, new AnimationComponent(pMenuButton, 1.f, 400, 251, 2));
	m_pReplayButton = new UIButton(20, 500, 70, 35, new AnimationComponent(pReplayButton, 1.f, 400, 251, 2));
	m_pQuitButton = new UIButton(20, 540, 70, 35, new AnimationComponent(pQuitButton, 1.f, 400, 251, 2));
}

void GameWinState::LoadBackgroundSounds()
{
	m_pSoundPlayer = new SoundPlayer();
	Mix_Music* pBackground = nullptr;
	m_pGameWorld->GetSoundLoadingSystem()->Load("Sounds/Music/WinBackgroundSound.mp3", pBackground);
	m_pSoundPlayer->SetSoundTrack(pBackground);
	m_pSoundPlayer->PlayMusic(false, -1);
}

void GameWinState::ProcessEndClickEvent(SDL_MouseButtonEvent* pData)
{
	UIButton* Menu = m_pMainMenuButton;
	UIButton* Replay = m_pReplayButton;
	UIButton* Quit = m_pQuitButton;

	switch (pData->button)
	{
	case SDL_BUTTON_LEFT:
	{
		if (Menu->HitTest(pData->x, pData->y))
		{
			// Set Event Transition
			m_pGameWorld->GetEventSystem()->SetTransition(true);

			Menu->Trigger();
			break;
		}

		if (Replay->HitTest(pData->x, pData->y))
		{
			// Set Event Transition
			m_pGameWorld->GetEventSystem()->SetTransition(true);

			Replay->Trigger();
			//m_transitionStarted = true;
			break;
		}

		if (Quit->HitTest(pData->x, pData->y))
		{
			Quit->Trigger();
			break;
		}
	}
	default:
		break;
	}
}

void GameWinState::ProcessEndMouseMotionEvent(SDL_MouseMotionEvent* pData)
{
	UIButton* Menu = m_pMainMenuButton;
	UIButton* Replay = m_pReplayButton;
	UIButton* Quit = m_pQuitButton;

	if (Menu->HitTest(pData->x, pData->y))
	{
		Menu->SetIsHighlighted(true);
	}
	else
	{
		Menu->SetIsHighlighted(false);
	}

	if (Replay->HitTest(pData->x, pData->y))
	{
		Replay->SetIsHighlighted(true);
	}
	else
	{
		Replay->SetIsHighlighted(false);
	}

	if (Quit->HitTest(pData->x, pData->y))
	{
		Quit->SetIsHighlighted(true);
	}
	else
	{
		Quit->SetIsHighlighted(false);
	}
}

void GameWinState::ProcessWindowEvent(SDL_WindowEvent* pData)
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
