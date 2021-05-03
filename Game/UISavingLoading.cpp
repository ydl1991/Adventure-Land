#include "UISavingLoading.h"
#include "EventSystem.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "UITextField.h"
#include "SimpleSDLGame.h"
#include "ImageLoadingSystem.h"
#include "AnimationComponent.h"
#include "UIButton.h"
#include "UITextureButton.h"
#include "ReadWriteSystem.h"
#include "BaseState.h"
#include "StateMachine.h"
#include "GameState.h"
#include "GameRunningState.h"
#include <iostream>

UISavingLoading::UISavingLoading(SimpleSDLGame* pGameWorld)
	: m_pGameWorld{ pGameWorld }
	, m_pInputBoxTexture{}
	, m_boarderBoxRect{}
	, m_pTitleTextField{ nullptr }
	, m_selectionBoxRect{}
	, m_pSelectionBoxTexture{ nullptr }
	, m_menu{ Menu::kOff }
	, m_pCurrentSurface{ nullptr }
	, m_isSlotOneDirty{false}
	, m_isSlotTwoDirty{false}
	, m_isSlotThreeDirty{false}
{
	m_pReadWriteSystem = pGameWorld->GetReadWriteSystem();
	Init();
}

UISavingLoading::~UISavingLoading()
{
	FreeScreenshot();

	m_pReadWriteSystem = nullptr;

	delete m_pTitleTextField;
	m_pTitleTextField = nullptr;

	delete m_pSaveButton;
	m_pSaveButton = nullptr;

	delete m_pSaveTextField;
	m_pSaveTextField = nullptr;

	delete m_pLoadButton;
	m_pLoadButton = nullptr;

	delete m_pLoadTextField;
	m_pLoadTextField = nullptr;

	delete m_pGameSlotOne;
	m_pGameSlotOne = nullptr;

	delete m_pGameOneSaveTime;
	m_pGameOneSaveTime = nullptr;

	delete m_pGameSlotTwo;
	m_pGameSlotTwo = nullptr;

	delete m_pGameTwoSaveTime;
	m_pGameTwoSaveTime = nullptr;

	delete m_pGameSlotThree;
	m_pGameSlotThree = nullptr;

	delete m_pGameThreeSaveTime;
	m_pGameThreeSaveTime = nullptr;
}

void UISavingLoading::Update(float deltaTime)
{
	if (m_pGameWorld->GetStateMachine()->GetCurrentState()->GetStateType() == BaseState::StateType::kGameMenuState)
		m_pSaveButton->GetAnimation()->PlayAnimation("Locked");
	else
		m_pSaveButton->GetAnimation()->PlayAnimation("Normal");

	if (m_pGameWorld->GetReadWriteSystem()->GetNumOfFileSaved() <= 0)
		m_pLoadButton->GetAnimation()->PlayAnimation("Locked");
	else
		m_pLoadButton->GetAnimation()->PlayAnimation("Normal");
}

void UISavingLoading::Render(SDL_Renderer* pRenderer)
{
	if (m_menu == Menu::kEntry)
	{
		SDL_RenderCopy(pRenderer, m_pInputBoxTexture, nullptr, &m_boarderBoxRect);
		m_pTitleTextField->Render(pRenderer);
		m_pSaveButton->Render(pRenderer);
		m_pSaveTextField->Render(pRenderer);
		m_pLoadButton->Render(pRenderer);
		m_pLoadTextField->Render(pRenderer);
	}
	else if (m_menu == Menu::kSave || m_menu == Menu::kLoad)
	{
		SDL_RenderCopy(pRenderer, m_pSelectionBoxTexture, nullptr, &m_selectionBoxRect);
		m_pGameSlotOne->Render(pRenderer);
		m_pGameOneSaveTime->Render(pRenderer);
		m_pGameSlotTwo->Render(pRenderer);
		m_pGameTwoSaveTime->Render(pRenderer);
		m_pGameSlotThree->Render(pRenderer);
		m_pGameThreeSaveTime->Render(pRenderer);
	}
}

void UISavingLoading::SetBoarderRect(int x, int y, int w, int h)
{
	m_boarderBoxRect.x = x;
	m_boarderBoxRect.y = y;
	m_boarderBoxRect.w = w;
	m_boarderBoxRect.h = h;
}

void UISavingLoading::TakeGameScreenshot()
{
	int w, h;
	SDL_GetRendererOutputSize(m_pGameWorld->GetRenderer(), &w, &h);

	m_pCurrentSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(m_pGameWorld->GetRenderer(), NULL, SDL_PIXELFORMAT_ARGB8888, m_pCurrentSurface->pixels, m_pCurrentSurface->pitch);
}

void UISavingLoading::SaveScreenshotToFile(int slot)
{
	// if invalid slot, return
	if (slot < 1 && slot > m_kNumOfSaveFiles)
	{
		std::cout << "Invalid Slot Number!" << std::endl;
		return;
	}

	if (m_pCurrentSurface)
	{
		// get the path name
		std::string path = s_kDirectory + std::string(m_kFileName[slot - 1]);
		// save screenshot under the current path name
		SDL_SaveBMP(m_pCurrentSurface, path.c_str());
		// delete surface
		FreeScreenshot();
		// delete old texture in texture map
		m_pGameWorld->GetImageLoadingSystem()->DeleteIndicatedTexture(path.c_str());

		// check which slot is dirty
		slot == 1 ? m_isSlotOneDirty = true : slot == 2 ? m_isSlotTwoDirty = true : m_isSlotThreeDirty = true;
	}
}

void UISavingLoading::FreeScreenshot()
{
	SDL_FreeSurface(m_pCurrentSurface);
	m_pCurrentSurface = nullptr;
}

void UISavingLoading::Init()
{
	ImageLoadingSystem* pIMGSystem = m_pGameWorld->GetImageLoadingSystem();
	SDL_Renderer* pRenderer = m_pGameWorld->GetRenderer();
	SDL_Texture* pTexture = nullptr;
	TextureSpriteSheet* pSprintSheet = nullptr;

	// Init Menu UI Box
	pIMGSystem->Load(pRenderer, "Assets/inputBox.png", pTexture);
	m_pInputBoxTexture = pTexture;
	pTexture = nullptr;
	m_boarderBoxRect = { 150,200,500,200 };

	// Init Menu Title
	TTF_Font* pFont = TTF_OpenFont("Fonts/Teko-SemiBold.ttf", 35);
	if (!pFont)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}
	SDL_Color white = { 255, 255, 255, 255 };
	m_pTitleTextField = new UITextField(230, 240, pFont, "What Would You Like To Do!", white, m_pGameWorld->GetRenderer());

	// Init Entry Menu Buttons
	// Save button
	pIMGSystem->Load(pRenderer, "Assets/Button/SaveLoad.png", pSprintSheet);
	InitSaveButton(pRenderer, pSprintSheet, pFont, white);
	pSprintSheet = nullptr;
	// Load button
	pIMGSystem->Load(pRenderer, "Assets/Button/SaveLoad.png", pSprintSheet);
	InitLoadButton(pRenderer, pSprintSheet, pFont, white);
	pSprintSheet = nullptr;

	// Init Selection Menu Box
	pIMGSystem->Load(pRenderer, "Assets/SaveLoad.png", pTexture);
	m_pSelectionBoxTexture = pTexture;
	pTexture = nullptr;
	m_selectionBoxRect = { 0,0, 300, 600 };

	// Init Game file slot
	TTF_Font* pSlotTitle = TTF_OpenFont("Fonts/Teko-SemiBold.ttf", 35);
	if (!pSlotTitle)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}
	
	// Init Game File Slots
	for (int i = 0; i < m_kNumOfSaveFiles; ++i)
	{
		std::string path = s_kDirectory + std::string(m_kFileName[i]);

		pIMGSystem->Load(pRenderer, path.c_str(), pTexture);

		if(i == 0)
			InitGameSlotOne(pRenderer, pTexture, pSlotTitle, white);
		else if(i == 1)
			InitGameSlotTwo(pRenderer, pTexture, pSlotTitle, white);
		else if(i == 2)
			InitGameSlotThree(pRenderer, pTexture, pSlotTitle, white);

		pTexture = nullptr;
	}
}

void UISavingLoading::InitSaveButton(SDL_Renderer* pRenderer, TextureSpriteSheet* pSpriteSheet, TTF_Font* pFont, SDL_Color color)
{
	// Init button and textfield
	m_pSaveButton = new UIButton(270, 300, 90, 40, new AnimationComponent(pSpriteSheet, 1, 90, 40, 2));
	m_pSaveButton->GetAnimation()->AddAnimationSequence("Normal", 0, 0);
	m_pSaveButton->GetAnimation()->AddAnimationSequence("Locked", 1, 1);
	m_pSaveButton->GetAnimation()->PlayAnimation("Normal");
	m_pSaveTextField = new UITextField(285, 297, pFont, "Save", color, pRenderer);

	// save button callback
	m_pSaveButton->SetCallback([this]() {
		m_pReadWriteSystem->UpdateSavedData();
		UpdateSavedScreenShot();
		m_menu = Menu::kSave;
	});
}

void UISavingLoading::InitLoadButton(SDL_Renderer* pRenderer, TextureSpriteSheet* pSpriteSheet, TTF_Font* pFont, SDL_Color color)
{
	// Init button and textfield
	m_pLoadButton = new UIButton(430, 300, 90, 40, new AnimationComponent(pSpriteSheet, 1, 90, 40, 2));
	m_pLoadButton->GetAnimation()->AddAnimationSequence("Normal", 0, 0);
	m_pLoadButton->GetAnimation()->AddAnimationSequence("Locked", 1, 1);
	m_pLoadTextField = new UITextField(445, 297, pFont, "Load", color, pRenderer);

	// load button callback
	m_pLoadButton->SetCallback([this]() {
		m_pReadWriteSystem->UpdateSavedData();
		UpdateSavedScreenShot();
		m_menu = Menu::kLoad;
	});
}

void UISavingLoading::InitGameSlotOne(SDL_Renderer* pRenderer, SDL_Texture* pTexture, TTF_Font* pFont, SDL_Color color)
{
	// Init button and textfield
	m_pGameSlotOne = new UITextureButton(60.f, 60.f, 180.f, 135.f);
	m_pGameSlotOne->SetTexture(pTexture);
	m_pGameOneSaveTime = new UITextField(20, 10, pFont, "Game One", color, pRenderer);

	// game one slot button callback
	m_pGameSlotOne->SetCallback([this]() {
		if (m_menu == Menu::kSave)
		{
			m_pReadWriteSystem->Save(1);
		}
		else if (m_menu == Menu::kLoad)
		{
			if (m_pGameWorld->GetStateMachine()->GetCurrentState()->GetStateType() == BaseState::StateType::kGameMenuState)
			{
				GameState* pState = new GameRunningState(m_pGameWorld, m_pReadWriteSystem->GetFileOne().m_playerType, "Player");
				m_pGameWorld->GetStateMachine()->SetCurrentState(pState);
				pState = nullptr;
				m_pReadWriteSystem->Load(1);
				// get back to game after click
				m_menu = Menu::kOff;
				return;
			}
			m_pReadWriteSystem->Load(1);
		}
		// get back to entry menu after click
		m_menu = Menu::kEntry;
	});
}

void UISavingLoading::InitGameSlotTwo(SDL_Renderer* pRenderer, SDL_Texture* pTexture, TTF_Font* pFont, SDL_Color color)
{
	// Init button and textfield
	m_pGameSlotTwo = new UITextureButton(60.f, 250.f, 180.f, 135.f);
	m_pGameSlotTwo->SetTexture(pTexture);
	m_pGameTwoSaveTime = new UITextField(20, 200, pFont, "Game Two", color, pRenderer);

	// game two slot button callback
	m_pGameSlotTwo->SetCallback([this]() {
		if (m_menu == Menu::kSave)
		{
			m_pReadWriteSystem->Save(2);
		}
		else if (m_menu == Menu::kLoad)
		{
			if (m_pGameWorld->GetStateMachine()->GetCurrentState()->GetStateType() == BaseState::StateType::kGameMenuState)
			{
				GameState* pState = new GameRunningState(m_pGameWorld, m_pReadWriteSystem->GetFileTwo().m_playerType, "Player");
				m_pGameWorld->GetStateMachine()->SetCurrentState(pState);
				pState = nullptr;
				m_pReadWriteSystem->Load(2);
				// get back to game after click
				m_menu = Menu::kOff;
				return;
			}
			m_pReadWriteSystem->Load(2);
		}
		// get back to entry menu after click
		m_menu = Menu::kEntry;
	});
}

void UISavingLoading::InitGameSlotThree(SDL_Renderer* pRenderer, SDL_Texture* pTexture, TTF_Font* pFont, SDL_Color color)
{
	// Init button and textfield
	m_pGameSlotThree = new UITextureButton(60.f, 440.f, 180.f, 135.f);
	m_pGameSlotThree->SetTexture(pTexture);
	m_pGameThreeSaveTime = new UITextField(20, 390, pFont, "Game Three", color, pRenderer);

	// game three slot button callback
	m_pGameSlotThree->SetCallback([this]() {
		if (m_menu == Menu::kSave)
		{
			m_pReadWriteSystem->Save(3);
		}
		else if (m_menu == Menu::kLoad)
		{
			if (m_pGameWorld->GetStateMachine()->GetCurrentState()->GetStateType() == BaseState::StateType::kGameMenuState)
			{
				GameState* pState = new GameRunningState(m_pGameWorld, m_pReadWriteSystem->GetFileThree().m_playerType, "Player");
				m_pGameWorld->GetStateMachine()->SetCurrentState(pState);
				pState = nullptr;
				m_pReadWriteSystem->Load(3);
				// get back to game after click
				m_menu = Menu::kOff;
				return;
			}
			m_pReadWriteSystem->Load(3);
		}
		// get back to entry menu after click
		m_menu = Menu::kEntry;
	});
}

void UISavingLoading::UpdateSavedScreenShot()
{
	std::string path = s_kDirectory + std::string(m_kFileName[0]);

	ImageLoadingSystem* pIMGSystem = m_pGameWorld->GetImageLoadingSystem();
	SDL_Renderer* pRenderer = m_pGameWorld->GetRenderer();
	SDL_Texture* pTexture = nullptr;

	// slot one dirty reload slot one
	if (m_isSlotOneDirty)
	{
		pIMGSystem->Load(pRenderer, path.c_str(), pTexture);
		if (pTexture)
		{
			m_pGameSlotOne->SetTexture(pTexture);
			pTexture = nullptr;
		}
		m_isSlotOneDirty = false;
	}

	// slot two dirty reload slot two
	if (m_isSlotTwoDirty)
	{
		path = s_kDirectory + std::string(m_kFileName[1]);
		pIMGSystem->Load(pRenderer, path.c_str(), pTexture);
		if (pTexture)
		{
			m_pGameSlotTwo->SetTexture(pTexture);
			pTexture = nullptr;
		}
		m_isSlotTwoDirty = false;
	}

	// slot three dirty reload slot three
	if (m_isSlotThreeDirty)
	{
		path = s_kDirectory + std::string(m_kFileName[2]);
		pIMGSystem->Load(pRenderer, path.c_str(), pTexture);
		if (pTexture)
		{
			m_pGameSlotThree->SetTexture(pTexture);
			pTexture = nullptr;
		}
		m_isSlotThreeDirty = false;
	}
}
