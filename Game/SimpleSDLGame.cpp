#include "SimpleSDLGame.h"
#include "ImageLoadingSystem.h"
#include "EventSystem.h"
#include "CollisionManager.h"
#include "StateMachine.h"
#include "GameState.h"
#include "GameRunningState.h"
#include "GameMenuState.h"
#include "SoundLoadingSystem.h"
#include "ReadWriteSystem.h"
#include "UISavingLoading.h"
#include "SDL_ttf.h"
#include <SDL.h>
#include <time.h>
#include <SDL_image.h>
#include <chrono>
#include <cmath>
#include <random>
#include <iostream>
#include <SDL_mixer.h>

SimpleSDLGame::SimpleSDLGame()
	: m_pRenderer{ nullptr }
	, m_pWindow{ nullptr }
	, m_pImageSystem{ nullptr }
	, m_pSoundSystem{ nullptr }
	, m_pEventSystem{ nullptr }
	, m_pCollisionManager{ nullptr }
	, m_pStateMachine{ nullptr }
	, m_pReadWriteSystem{ nullptr }
	, m_pSavingLoadingMenu{ nullptr }
{
	//
}

SimpleSDLGame::~SimpleSDLGame()
{
	//
}

//----------------------------------------------------------------//
//  call to initialize SDL windows and renderer to draw the art
//----------------------------------------------------------------//
void SimpleSDLGame::Init()
{
	// set allow screen and music
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		std::cout << "Failed to initialize SDL" << std::endl;
		return;
	}

	// create window
	m_pWindow = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_kWindowWidth, m_kWindowHeight, SDL_WINDOW_SHOWN);
	// check if window create successfully
	if (m_pWindow != nullptr)
	{
		std::cout << "SDL_CreateWindow() succeeded" << std::endl;
	}
	else
	{
		std::cout << "SDL_CreateWindow() failed. Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// create renderer
	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
	// check if creation is succeeded
	if (m_pRenderer != nullptr)
	{
		std::cout << "SDL_CreateRenderer() succeeded" << std::endl;
	}
	else
	{
		std::cout << "SDL_CreateRenderer() failed" << SDL_GetError() << std::endl;
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();
		return;
	}	
	
	// Open SDL Audio
	int errorCode = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
	if (errorCode != 0)
	{
		std::cout << "Mix_OpenAudio() failed. Error: " << Mix_GetError() << std::endl;
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();
		return;
	}

	// Init Mixer Audio
	int audioFlag = MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_OGG;
	errorCode = Mix_Init(audioFlag);
	if (errorCode != audioFlag)
	{
		std::cout << "Mix_Init() failed. Error: " << Mix_GetError() << std::endl;
		Mix_CloseAudio();
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();
		return;
	}

	// init TTF
	TTF_Init();

	// initialize all the systems
	InitSystems();

	// initialize the begining game menu here
	InitGameMenu();

	std::cout << "Successfully initialized SDL!" << std::endl;
}

// call to shut down windows
void SimpleSDLGame::ShutDown()
{
	// delete save and load system
	delete m_pSavingLoadingMenu;
	m_pSavingLoadingMenu = nullptr;

	delete m_pReadWriteSystem;
	m_pReadWriteSystem = nullptr;

	// delete state machine
	delete m_pStateMachine;
	m_pStateMachine = nullptr;

	// delete event system
	delete m_pEventSystem;
	m_pEventSystem = nullptr;

	// delete image system
	delete m_pImageSystem;
	m_pImageSystem = nullptr;

	// delete sound system
	delete m_pSoundSystem;
	m_pSoundSystem = nullptr;

	// delete collision manager
	delete m_pCollisionManager;
	m_pCollisionManager = nullptr;

	// distroy renderer when game end
	SDL_DestroyRenderer(m_pRenderer);
	m_pWindow = nullptr;

	// distroy window when game end
	SDL_DestroyWindow(m_pWindow);
	m_pRenderer = nullptr;

	// Quit Mixer
	Mix_CloseAudio();

	// Quit TTF
	TTF_Quit();

	// quit window
	SDL_Quit();

	// Terminate Program
	exit(0);
}

/*******************************************************/
//                 Major Game Update
//	1. Delta time calculation
//  2. Game State Update
//  3. Render Game State
/*******************************************************/
//call to update each frame
void SimpleSDLGame::Update()
{
	// variable determines end game event
	bool shouldQuit = false;
	// start frame count
	auto lastFrameTime = std::chrono::high_resolution_clock::now();

	while (!shouldQuit)
	{	
		/*******************************************************/
		//                 Set Time Duration
		/*******************************************************/
		// end frame count
		auto thisFrameTime = std::chrono::high_resolution_clock::now();
		// calculate frame duration
		std::chrono::duration<float> lastFrameDuration = thisFrameTime - lastFrameTime;
		float deltaTime = lastFrameDuration.count();
		// change frame starting time to the current frame ending time
		lastFrameTime = thisFrameTime;

		/*******************************************************/
		//				Process Game State Events
		/*******************************************************/
		shouldQuit = m_pStateMachine->ProcessEvents();

		/*******************************************************/
		//             Update Game State && Render
		/*******************************************************/
		if (m_pStateMachine)
		{
			m_pStateMachine->RenderCurrentState(m_pRenderer);
			m_pStateMachine->Update(deltaTime);
		}
	}
}

/////////////////////////////////
// Init Game Systems here
/////////////////////////////////
void SimpleSDLGame::InitSystems()
{
	m_pCollisionManager = new CollisionManager();
	m_pImageSystem = new ImageLoadingSystem();
	m_pSoundSystem = new SoundLoadingSystem();
	m_pEventSystem = new EventSystem();
	m_pStateMachine = new StateMachine();
	m_pReadWriteSystem = new ReadWriteSystem(this);
	m_pSavingLoadingMenu = new UISavingLoading(this);
}

/////////////////////////////////
// create main menu state here
/////////////////////////////////
void SimpleSDLGame::InitGameMenu()
{
	GameState* pState = new GameMenuState(this);
	m_pStateMachine->SetCurrentState(pState);
	pState = nullptr;
}

