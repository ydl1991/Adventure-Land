#include "GameRunningState.h"
#include "Player.h"
#include "SimpleSDLGame.h"
#include "ImageLoadingSystem.h"
#include "EventSystem.h"
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "GameWinState.h"
#include "StaticBoarder.h"
#include "SpawnerSystem.h"
#include "GameLoseState.h"
#include "WorldBackground.h"
#include "GameEndingState.h"
#include "StateMachine.h"
#include "WinZone.h"
#include "UIHud.h"
#include "SoundPlayer.h"
#include "SoundLoadingSystem.h"
#include "EnemyBoss.h"
#include "SDL.h"
#include "FireBall.h"
#include "DamageZone.h"
#include "HealingZone.h"
#include "ReadWriteSystem.h"
#include "UISavingLoading.h"
#include "Thunder.h"
#include "UIButton.h"
#include "UITextureButton.h"
#include "Projectile.h"
#include <iostream>

//-----------------------------
//	Constructor
//-----------------------------
GameRunningState::GameRunningState(SimpleSDLGame* pGameWorld, int playerType, std::string pName)
	: m_winCallback{nullptr}
	, m_loseCallback{nullptr}
	, m_pSoundPlayer{nullptr}
	, m_pBoss{nullptr}
	, m_playerType{ playerType }
	, m_pWinZone{nullptr}
	, m_pressCtrl{ false }
	, m_pressQ{ false }
	, m_fireTimer{0}
	, m_fireComboIndicator{0}
{
	m_pGameWorld = pGameWorld;
	m_pGameWorld->GetEventSystem()->ClearNormalEvents();
	m_pGameWorld->GetCollisionManager()->RemoveAllColliderAndPair();

	//m_pSavingLoadingMenu = m_pGameWorld->GetSavingLoadingMenu();
	m_pStateMachine = m_pGameWorld->GetStateMachine();
	m_type = BaseState::StateType::kGameRunningState;
	CreatePlayer(playerType, pName);
	CreateBackground();
	CreateBoarder();
	CreateHUD(playerType);
	LoadBackgroundSounds();

	// Add the object as Event Listener
	m_pGameWorld->GetEventSystem()->AddInputEventListener(this);
}

//-----------------------------
//	Destructor
//-----------------------------
GameRunningState::~GameRunningState()
{
	//
	// Remove this object from Event Listener List
	m_pGameWorld->GetEventSystem()->RemoveInputEventListener(this);

	ClearAll();

}

//-----------------------------
//	Init Game State
//-----------------------------
void GameRunningState::EnterState()
{
	m_pSpawnerSystem = new SpawnerSystem(this);

	InitFireballTrace();
	m_pSpawnerSystem->CheckSpawnBoss();
	CreateFireBalls();

	// assign win callback
	m_winCallback = [this] {
		GameState* pState = new GameWinState(m_pGameWorld, m_playerType, m_pPlayer->GetName());
		m_pStateMachine->SetCurrentState(pState);
		pState = nullptr;
	};
	// assign lose callback
	m_loseCallback = [this] {
 		GameState* pState = new GameLoseState(m_pGameWorld, m_playerType, m_pPlayer->GetName());
		m_pStateMachine->SetCurrentState(pState);
		pState = nullptr;
	};
}

//-----------------------------
//	Update Current State
//-----------------------------
void GameRunningState::Update(float deltaTime)
{
	if (m_pGameWorld->GetSavingLoadingMenu()->GetCurrentMenu() == UISavingLoading::Menu::kOff)
	{
		CreateWinZone();													// check if it's time to create win zone
		if(m_pWinZone)
			m_pWinZone->Tick(deltaTime);									// check winzone update

		m_pWorldBackground->Update(deltaTime);								// check map camera update

		m_pPlayer->Tick(deltaTime);											// update player
		m_pBoss->Tick(deltaTime);											// update boss

		UpdateObjectList(deltaTime);										// update object list
		RemoveInvalidObject();												// check and delete invalid object after new update

		UpdateFireBalls(deltaTime);											// update object list

		UpdateWeaponCombo(deltaTime);
		CheckFireProjectile(deltaTime);

		m_pGameWorld->GetCollisionManager()->ProcessCollidingPair();		// process colliding pair
		m_pSpawnerSystem->Update(deltaTime);								// check spawning
		m_pHud->Update(deltaTime);											// Update HUD

		// check win
		if (m_pPlayer->HasWon())
		{
			Mix_FadeOutMusic(1000);
			m_winCallback();
			return;
		}

		// check lose
		if (!m_pPlayer || !m_pPlayer->IsValid())
		{
			Mix_FadeOutMusic(1000);
			m_loseCallback();
			return;
		}
	}
	
	m_pGameWorld->GetSavingLoadingMenu()->Update(deltaTime);								// update saving and loading menu
}

void GameRunningState::ExitState()
{
	//
}

//-----------------------------
//	Process Current State's Event
//-----------------------------
bool GameRunningState::ProcessEvents()
{
	// process player related SDL events
	m_pGameWorld->GetEventSystem()->ProcessNormalEvents();
	m_pGameWorld->GetEventSystem()->ProcessSDLInputEvents();

	return (m_pressCtrl && m_pressQ);
}

//---------------------------------------------
// render valid object and remove invalid object
//---------------------------------------------
void GameRunningState::RenderCurrentState(SDL_Renderer* pRenderer)
{
	SDL_RenderClear(pRenderer);										// clear current screen

	m_pWorldBackground->Render(pRenderer);							// render background
	if(m_pWinZone)
		m_pWinZone->Render(pRenderer);								// render winzone
	m_pPlayer->Render(pRenderer);									// render player

	RenderObjectList(pRenderer);									// render all object
	RenderFireballs(pRenderer);										// render fire balls
	RenderBullets(pRenderer);

	m_pBoss->Render(pRenderer);										// render boss

	m_pHud->Render(pRenderer);										// render HUD
	m_pGameWorld->GetSavingLoadingMenu()->Render(pRenderer);		// render saving loading menu

	SDL_RenderPresent(pRenderer);									// present on screen
}

void GameRunningState::ProcessInputEvent(SDL_Event& evt)
{
	switch (evt.type)
	{
		// keyboard events
	case SDL_KEYDOWN:
		ProcessPressKeyBoardEvent(&evt.key);
		break;
	case SDL_KEYUP:
		ProcessReleaseKeyBoardEvent(&evt.key);
		break;
		// mouse events
	case SDL_MOUSEBUTTONDOWN:
		ProcessMouseClickEvent(&evt.button);
		break;
		// window events
	case SDL_WINDOWEVENT:
		ProcessWindowEvent(&evt.window);
		break;
	}
}

//--------------------------------
//	Add new object in object list
//--------------------------------
void GameRunningState::AddObject(ObjectBase* pObject)
{
	m_allObjects.emplace_back(pObject);
}

//---------------------------------------
//	Clear state for game loading
//---------------------------------------
void GameRunningState::LoadGamePreparation()
{
	// before load game, clear out all current enemy and walls and damage healing zones
	for (auto it = m_allObjects.begin(); it != m_allObjects.end(); ++it)
	{
		// move on if it's a static boarder
		StaticBoarder* pBoarder = dynamic_cast<StaticBoarder*>(*it);
		if (pBoarder)
			continue;

		// if object is a enemy, clear it
		Enemy* pEnemy = dynamic_cast<Enemy*>(*it);
		if (pEnemy)
		{
			pEnemy->Invalidate();
			continue;
		}

		// if object is a wall, clear it
		Wall* pWall = dynamic_cast<Wall*>(*it);
		if (pWall)
		{
			pWall->Invalidate();
			continue;
		}

		// if object is a damage or healing zone, clear it
		DamageZone* pDamageZone = dynamic_cast<DamageZone*>(*it);
		if (pDamageZone)
		{
			pDamageZone->Invalidate();
			continue;
		}
		HealingZone* pHealingZone = dynamic_cast<HealingZone*>(*it);
		if (pHealingZone)
		{
			pHealingZone->Invalidate();
			continue;
		}
	}

	for (int i = 0; i < m_fireBalls.size(); ++i)
	{
		m_fireBalls[i]->Invalidate();
	}

	if(m_pBoss && m_pBoss->GetThunder())
		m_pBoss->GetThunder()->SetActiveStatus(false);

	delete m_pWinZone;
	m_pWinZone = nullptr;
}

//---------------------------------------
//	Remove Invalid Object in object list
//---------------------------------------
void GameRunningState::RemoveInvalidObject()
{
	// check and delete invalid object after new update
	auto it = m_allObjects.begin();
	while (it != m_allObjects.end())
	{
		if ((*it)->IsValid() == false)
		{
			delete (*it);
			(*it) = nullptr;
			it = m_allObjects.erase(it);
		}
		else
			++it;
	}
}

//-----------------------------
//	Clear memory
//-----------------------------
void GameRunningState::ClearAll()
{
	for (auto it = m_allObjects.begin(); it != m_allObjects.end(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}
	m_allObjects.clear();

	for (auto it = m_fireBalls.begin(); it != m_fireBalls.end(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}
	m_fireBalls.clear();

	for (auto it = m_WeaponCompo.begin(); it != m_WeaponCompo.end(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}
	m_WeaponCompo.clear();

	delete m_pPlayer;
	m_pPlayer = nullptr;

	delete m_pBoss;
	m_pBoss = nullptr;

	delete m_pWinZone;
	m_pWinZone = nullptr;

	delete m_pSpawnerSystem;
	m_pSpawnerSystem = nullptr;

	delete m_pWorldBackground;
	m_pWorldBackground = nullptr;

	delete m_pHud;
	m_pHud = nullptr;

	delete m_pSoundPlayer;
	m_pSoundPlayer = nullptr;

	m_pStateMachine = nullptr;

	m_pGameWorld->GetEventSystem()->ClearNormalEvents();

	m_pGameWorld = nullptr;
}

void GameRunningState::InternalClearCurrentBullets()
{
	for (auto it = m_WeaponCompo.begin(); it != m_WeaponCompo.end(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}
	m_WeaponCompo.clear();
}

//-----------------------------
//	Create player object
//-----------------------------
void GameRunningState::CreatePlayer(int playerType, std::string pName)
{
	// create player and initialize
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = { 350.f,500.f };
	objectSpawnInfo.m_pGameRunningState = this;
	m_pPlayer = new Player(objectSpawnInfo, pName);

	TextureSpriteSheet* pSpriteSheet = nullptr;

	// store zone filename
	const char* playerImages[2] = { "Assets/CharacterAnimation/animation1.png", "Assets/CharacterAnimation/warplane1.png" };
	// choose character skin
	m_pGameWorld->GetImageLoadingSystem()->Load(m_pGameWorld->GetRenderer(), playerImages[playerType], pSpriteSheet);
	// create animation component and attach to player
	if (playerType == 0)
		m_pPlayer->AddAnimationComponent(new AnimationComponent(pSpriteSheet, 12.f, 80, 100, 18));
	else
		m_pPlayer->AddAnimationComponent(new AnimationComponent(pSpriteSheet, 6.f, 64, 64, 16));

	// add collision component
	m_pPlayer->AddCollisionComponent(new CollisionComponent(m_pPlayer, m_pPlayer->GetCollisionBox(), m_pGameWorld->GetCollisionManager(), CollisionComponent::CollisionState::kCanBeOverlap));

	// initialize animation
	m_pPlayer->Init(playerType);
}

//-----------------------------
//	Create background texture
//-----------------------------
void GameRunningState::CreateBackground()
{
	// load texture
	SDL_Texture* pBackground = nullptr;
	m_pGameWorld->GetImageLoadingSystem()->Load(m_pGameWorld->GetRenderer(), "Assets/background.bmp", pBackground);
	// create background
	m_pWorldBackground = new WorldBackground(pBackground, 0.f, 3342.f, (float)m_pGameWorld->GetWidth(), (float)m_pGameWorld->GetHeight());
}

//-----------------------------
// call to spawn win zone
//-----------------------------
void GameRunningState::CreateWinZone()
{
	if (m_pWorldBackground->GetCameraDimention().y < 250.f && !m_pWinZone)
	{
		TextureSpriteSheet* pSpriteSheet = nullptr;
		m_pGameWorld->GetImageLoadingSystem()->Load(m_pGameWorld->GetRenderer(), "Assets/ZoneAnimation/winZone.png", pSpriteSheet);

		// create win zone
		WinZone* pWinZone = new WinZone(300.f, -200.f, this);

		// create animation component and attach to zone object
		pWinZone->AddAnimationComponent(new AnimationComponent(pSpriteSheet, 20.f, 320, 320, 8));

		// Collision Dimension
		pWinZone->AddCollisionComponent(new CollisionComponent(pWinZone, pWinZone->GetCollisionBox(), m_pGameWorld->GetCollisionManager(), CollisionComponent::CollisionState::kCanBeOverlap));

		// initialize animation
		pWinZone->Init();

		//emplace back to object list
		m_pWinZone = pWinZone;
	}
}

//-----------------------------
//	Create Static boarder walls
//-----------------------------
void GameRunningState::CreateBoarder()
{
	// set boarder walls to prevent go off boarder
	// create boarder walls and store in object list
	StaticBoarder* pWallLeft = new StaticBoarder(this, -3.f, 0);
	StaticBoarder* pWallRight = new StaticBoarder(this, (float)(m_pGameWorld->GetWidth()), 0);

	// create collision components
	pWallLeft->AddCollisionComponent(new CollisionComponent(pWallLeft, pWallLeft->GetCollisionBox(), m_pGameWorld->GetCollisionManager(), CollisionComponent::CollisionState::kPhysicallyCollidable));
	pWallRight->AddCollisionComponent(new CollisionComponent(pWallRight, pWallRight->GetCollisionBox(), m_pGameWorld->GetCollisionManager(), CollisionComponent::CollisionState::kPhysicallyCollidable));

	// make it valid and add collision component
	pWallLeft->Validate();
	// make it valid and add collision component
	pWallRight->Validate();

	// add to obejct list
	AddObject(pWallLeft);
	AddObject(pWallRight);
}

//-----------------------------
//	Create and Init HUD
//-----------------------------
void GameRunningState::CreateHUD(int playerType)
{
	m_pHud = new UIHud(this, playerType);
	m_pHud->SetLayoutRect(3,1);
}

void GameRunningState::LoadBackgroundSounds()
{
	m_pSoundPlayer = new SoundPlayer();
	Mix_Music* pBackground = nullptr;
	m_pGameWorld->GetSoundLoadingSystem()->Load("Sounds/Music/GameBackgroundSound.mp3", pBackground);
	m_pSoundPlayer->SetSoundTrack(pBackground);
	m_pSoundPlayer->PlayMusic(true, -1, 2000);
}

void GameRunningState::CreateFireBalls()
{
	CollisionManager* pManager = m_pGameWorld->GetCollisionManager();

	for (int i = 0; i < 10; ++i)
	{
		// Load spritesheet
		TextureSpriteSheet* pAnimation = nullptr;
		m_pGameWorld->GetImageLoadingSystem()->Load(m_pGameWorld->GetRenderer(), "Assets/Boss/fireball.png", pAnimation);
		float fireLocationParam = m_circleParameters[i];

		// calculate relative position around object center
		float x, y;
		x = (cos(fireLocationParam * 6.3f) * (m_pBoss->GetWidth() * 0.7f));
		y = (sin(fireLocationParam * 6.3f) * (m_pBoss->GetWidth() * 0.7f));

		// calculate object rotation and moving direction
		Vector2D direction(x, y);
		direction = direction.NormalizeVector(0);
		direction.Rotate(direction.m_x, direction.m_y);

		// spawn object
		ObjectSpawnInfo objectSpawnInfo;
		objectSpawnInfo.m_spawnLocation = { 0, 0};
		objectSpawnInfo.m_pSpriteSheet = pAnimation;
		objectSpawnInfo.m_pCollisionManager = pManager;
		objectSpawnInfo.m_pGameRunningState = this;
		FireBall* pFireBall = new FireBall(objectSpawnInfo, direction);

		// init object
		pFireBall->Init();

		// add object to fireball list
		m_fireBalls.emplace_back(pFireBall);
	}
}

void GameRunningState::InitFireballTrace()
{
	float circlePoint = 0.f;
	for (int i = 0; i < 10; ++i)
	{
		m_circleParameters[i] = circlePoint;
		circlePoint += 0.1f;
	}
}

//----------------------------------------------------------------
//	clear out enemy on screen if player got explision potion item
//----------------------------------------------------------------
void GameRunningState::ClearOutScreen()
{
	for (auto it = m_allObjects.begin(); it != m_allObjects.end(); ++it)
	{
		if ((*it)->IsValid())
		{
			ObjectBase::ObjectType type = (*it)->GetType();
			if (type == ObjectBase::ObjectType::kEnemy || type == ObjectBase::ObjectType::kWall)
			{
				(*it)->Invalidate();
			}
		}
	}
}

void GameRunningState::ClearCurrentBullets()
{
	InternalClearCurrentBullets();
}

void GameRunningState::UpdateObjectList(float deltaTime)
{
	//update all object in object lists
	for (auto it = m_allObjects.begin(); it != m_allObjects.end(); ++it)
	{
		(*it)->Tick(deltaTime);
	}
}

void GameRunningState::UpdateFireBalls(float deltaTime)
{
	for (auto it : m_fireBalls)
	{
		it->Tick(deltaTime);
	}
}

void GameRunningState::UpdateWeaponCombo(float deltaTime)
{
	for (int i = 0; i < m_WeaponCompo.size(); ++i)
	{
		m_WeaponCompo[i]->Update(deltaTime);
	}
}

void GameRunningState::CheckFireProjectile(float deltaTime)
{
	m_fireTimer -= deltaTime;

	if (m_fireTimer <= 0 && m_pPlayer->GetFireStatus())
	{
		// reset timer
		m_fireTimer = (float)(1 / m_pPlayer->GetFireRate());

		if (m_fireComboIndicator >= 10)
			m_fireComboIndicator = 0;

		Vector2D firePoint{};
		Vector2D firePointTwo{};

		// Fire
		switch (m_pPlayer->GetWeaponLevel())
		{
		case 1:
		{
			firePoint = { m_pPlayer->GetX() + (m_pPlayer->GetDimention().m_x / 2.f) , m_pPlayer->GetY() - 25.f };
			m_WeaponCompo[m_fireComboIndicator]->Reset(firePoint);

			// load and play shooting sound
			Mix_Chunk* pShootSound = nullptr;
			m_pGameWorld->GetSoundLoadingSystem()->Load("Sounds/Chunk/Shoot.wav", pShootSound);
			pShootSound->volume = 50;
			m_pSoundPlayer->PlayChunk(pShootSound, false, 0, 1, 3);
		}
			break;
		case 2:
		{
			firePoint = { m_pPlayer->GetX() - 50.f , m_pPlayer->GetY() - 25.f };
			m_WeaponCompo[m_fireComboIndicator]->Reset(firePoint);

			// load and play shooting sound
			Mix_Chunk* pShootSound = nullptr;
			m_pGameWorld->GetSoundLoadingSystem()->Load("Sounds/Chunk/magicShoot.wav", pShootSound);
			pShootSound->volume = 50;
			m_pSoundPlayer->PlayChunk(pShootSound, false, 0, 1, 3);
		}
			break;
		case 3:
		{
			firePoint = { m_pPlayer->GetX() - 50.f , m_pPlayer->GetY() - 25.f };
			firePointTwo = { m_pPlayer->GetX() + (m_pPlayer->GetDimention().m_x / 2.f) - 30.f, m_pPlayer->GetY() - 25.f };
			m_WeaponCompo[m_fireComboIndicator]->LevelThreeReset(firePoint, firePointTwo);

			// load and play shooting sound
			Mix_Chunk* pShootSound = nullptr;
			m_pGameWorld->GetSoundLoadingSystem()->Load("Sounds/Chunk/magicShoot.wav", pShootSound);
			pShootSound->volume = 50;
			m_pSoundPlayer->PlayChunk(pShootSound, false, 0, 1, 3);
		}
			break;
		default:
			break;
		}
		
		++m_fireComboIndicator;
	}
}

void GameRunningState::RenderObjectList(SDL_Renderer* pRenderer)
{
	for (auto it : m_allObjects)
	{
		if (it->IsValid())
			it->Render(pRenderer);
	}
}

void GameRunningState::RenderFireballs(SDL_Renderer* pRenderer)
{
	for (auto it : m_fireBalls)
	{
		if (it->IsValid())
			it->Render(pRenderer);
	}
}

void GameRunningState::RenderBullets(SDL_Renderer* pRenderer)
{
	for (auto it : m_WeaponCompo)
	{
		it->Render(pRenderer);
	}
}

void GameRunningState::ProcessPressKeyBoardEvent(SDL_KeyboardEvent* pData)
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
		{
			pMenu->FreeScreenshot();
			pMenu->SetCurrentMenu(UISavingLoading::Menu::kOff);
		}
		else
		{
			pMenu->TakeGameScreenshot();
			pMenu->SetCurrentMenu(UISavingLoading::Menu::kEntry);
		}
		break;
	}
	// case to quit the game
	case SDL_SCANCODE_LCTRL:
		std::cout << "Successfully Pressed " << keyName << std::endl;
		m_pressCtrl = true;
		break;
	case SDL_SCANCODE_Q:
		std::cout << "Successfully Pressed " << keyName << std::endl;
		m_pressQ = true;
		break;
	default:
		break;
	}
}

void GameRunningState::ProcessReleaseKeyBoardEvent(SDL_KeyboardEvent* pData)
{
	if (pData->repeat)
		return;

	switch (pData->keysym.scancode)
	{
		// reset quit conditions
	case SDL_SCANCODE_LCTRL:
		m_pressCtrl = false;
		break;
	case SDL_SCANCODE_Q:
		m_pressQ = false;
		break;
	default:
		break;
	}
}

void GameRunningState::ProcessMouseClickEvent(SDL_MouseButtonEvent* pData)
{
	switch (pData->button)
	{
		// motion moving condition
	case SDL_BUTTON_LEFT:
	{
		UISavingLoading* pMenu = m_pGameWorld->GetSavingLoadingMenu();
		ReadWriteSystem* pReadWriteSystem = m_pGameWorld->GetReadWriteSystem();
		
		if (pMenu && pMenu->GetCurrentMenu() == UISavingLoading::Menu::kEntry)
		{
			if (pMenu->GetSaveButton()->HitTest(pData->x, pData->y))
			{
				pMenu->GetSaveButton()->Trigger();
				return;
			}

			if (pReadWriteSystem->GetNumOfFileSaved() > 0 && pMenu->GetLoadButton()->HitTest(pData->x, pData->y))
			{
				pMenu->GetLoadButton()->Trigger();
				return;
			}
		}
		// if open save menu and clicked on game slot, save the screenshot and file
		else if (pMenu && pMenu->GetCurrentMenu() == UISavingLoading::Menu::kSave)
		{
			if (pMenu->GetGameOneSlot()->HitTest(pData->x, pData->y))
			{
				pMenu->GetGameOneSlot()->Trigger();
				pMenu->SaveScreenshotToFile(1);
				return;
			}

			if (pMenu->GetGameTwoSlot()->HitTest(pData->x, pData->y))
			{
				pMenu->GetGameTwoSlot()->Trigger();
				pMenu->SaveScreenshotToFile(2);
				return;
			}

			if (pMenu->GetGameThreeSlot()->HitTest(pData->x, pData->y))
			{
				pMenu->GetGameThreeSlot()->Trigger();
				pMenu->SaveScreenshotToFile(3);
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

void GameRunningState::ProcessWindowEvent(SDL_WindowEvent* pData)
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
