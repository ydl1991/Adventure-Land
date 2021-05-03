#include "SpawnerSystem.h"
#include "TextureSpriteSheet.h"
#include "GameRunningState.h"
#include "AnimationComponent.h"
#include "CollisionComponent.h"
#include "ImageLoadingSystem.h"
#include "WorldBackground.h"
#include "SoundLoadingSystem.h"
#include "Vector2D.h"
#include "CollisionManager.h"
#include "SoundPlayer.h"
#include "Player.h"
#include "DamageZone.h"
#include "HealingZone.h"
#include "Dragon.h"
#include "Tree.h"
#include "Zombie.h"
#include "Projectile.h"
#include "Wall.h"
#include "SimpleSDLGame.h"
#include "EventSystem.h"
#include "EnemyBoss.h"
#include "RandomNumberGenerator.h"
#include "FireBall.h"
#include "ObjectBase.h"
#include "WinZone.h"
#include "WeaponPowerUp.h"
#include "MagicWave.h"
#include "BlueMagicBullet.h"
#include "SmallOpticalWave.h"
#include "SuperBomb.h"
#include <time.h>
#include <iostream>
#include <SDL.h>
#include <random>
#include <SDL_mixer.h>

//---------------------------
// spawning probablity chart
//---------------------------
const SpawnerSystem::SpawningTypeProbability SpawnerSystem::s_objectSpawningProbability[(int)SpawningType::kNumberOfType] =
{
	SpawnerSystem::SpawningTypeProbability(650, SpawningType::kEnemy),
	SpawnerSystem::SpawningTypeProbability(150, SpawningType::kWall),
	SpawnerSystem::SpawningTypeProbability(50, SpawningType::kZone),
	SpawnerSystem::SpawningTypeProbability(50, SpawningType::kItem),
};

//-----------------
// Constructor
//-----------------
SpawnerSystem::SpawnerSystem(GameRunningState* pGameRunningState)
	: m_pGameRunningState(pGameRunningState)
	//, m_projectileSpawnTimer{0}
	, m_maxProbability{0}
	, m_objectSpawnTimer{1.5f}
	, m_bossShowingTimer{5.f}
	//, m_fireComboIndicator{0}
{
	// Calculate max probability for spawn
	CalculateMaxProbability();
	// Init different RNGs
	InitRNGs();
	// Check Weapon combo init
	CheckWeaponComboInit();
}

SpawnerSystem::~SpawnerSystem()
{
	//
	delete m_pWidthGenerator;
	m_pWidthGenerator = nullptr;

	delete m_pSpawningProbabilityRNG;
	m_pSpawningProbabilityRNG = nullptr;

	delete m_pSpawningTimerRNG;
	m_pSpawningTimerRNG = nullptr;

	delete m_pZoneAndItemTypeRNG;
	m_pZoneAndItemTypeRNG = nullptr;

	delete m_pEnemyTypeRNG;
	m_pEnemyTypeRNG = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Spawn function one
// Spawns object that do not require to pass in a event system pointer
// Parameters:
//	coordinate x, coordinate y, object spritesheet
//////////////////////////////////////////////////////////////////////////
template<class SpawnType>
SpawnType* SpawnerSystem::SpawnObject(float x, float y, TextureSpriteSheet* pSpriteSheet)
{
	// create object
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = { x,y };
	objectSpawnInfo.m_pSpriteSheet = pSpriteSheet;
	objectSpawnInfo.m_pCollisionManager = m_pGameRunningState->GetWorld()->GetCollisionManager();
	objectSpawnInfo.m_pGameRunningState = m_pGameRunningState;

	SpawnType* pTemp = new SpawnType(objectSpawnInfo);
	pTemp->Init();

	// set temporary pointers to nullptr
	return pTemp;
}

template<class SpawnType>
SpawnType* SpawnerSystem::SpawnObject(float x, float y, Vector2D direction, TextureSpriteSheet* pSpriteSheet)
{

	// create object
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = { x,y };
	objectSpawnInfo.m_pSpriteSheet = pSpriteSheet;
	objectSpawnInfo.m_pCollisionManager = m_pGameRunningState->GetWorld()->GetCollisionManager();
	objectSpawnInfo.m_pGameRunningState = m_pGameRunningState;

	SpawnType* pTemp = new SpawnType(objectSpawnInfo, direction);
	pTemp->Init();

	// set temporary pointers to nullptr
	return pTemp;
}

///////////////////////////////////////////////////////////////////////////////////////
// Spawn function two
// Spawns object that do require to pass in a event system pointer
// Parameters:
//	coordinate x, coordinate y, event system pointer, object spritesheet, running state
///////////////////////////////////////////////////////////////////////////////////////
template<class SpawnType>
SpawnType* SpawnerSystem::SpawnObject(float x, float y, EventSystem* pEventSystem, TextureSpriteSheet* pSpriteSheet)
{
	// spawn object
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = {x,y};
	objectSpawnInfo.m_pEventSystem = pEventSystem;
	objectSpawnInfo.m_pSpriteSheet = pSpriteSheet;
	objectSpawnInfo.m_pCollisionManager = m_pGameRunningState->GetWorld()->GetCollisionManager();
	objectSpawnInfo.m_pGameRunningState = m_pGameRunningState;

	SpawnType* pTemp = new SpawnType(objectSpawnInfo);
	pTemp->Init();

	// set temporary pointers to nullptr
	return pTemp;
}

void SpawnerSystem::SpawnObjectByProbability()
{
	if (m_objectSpawnTimer <= 0)
	{
		// Generate a random probability
		int roll = m_pSpawningProbabilityRNG->GenerateRandomInt();
		// Set Spawn object index to 0
		int rollingIndex = 0;
		// Start randomly generate which object to spawn
		while (true)
		{
			// if rolled number minus the index object's probability score is less than 0, we spawn this object
			roll -= s_objectSpawningProbability[rollingIndex].first;
			if (roll < 0)
				break;
			// otherwise move to check next object
			++rollingIndex;
		}

		// spawning
		switch (s_objectSpawningProbability[rollingIndex].second)
		{
		case SpawningType::kEnemy:
			CheckSpawnEnemy();
			break;
		case SpawningType::kWall:
			CheckSpawnWall();
			break;
		case SpawningType::kZone:
			CheckSpawnZone();
			break;
		case SpawningType::kItem:
			CheckSpawnItem();
			break;
		default:
			break;
		}

		// Reset Object Spawning Timer
		float spawnTimer = m_pSpawningTimerRNG->GenerateRandomFloat();
		SetObjectSpawnTimer(spawnTimer);
	}
}

void SpawnerSystem::Update(float deltaTime)
{
	// calculate time to spawn
	if (!m_pGameRunningState->GetBoss()->IsValid())
	{
		m_bossShowingTimer -= deltaTime;
		if (m_bossShowingTimer < 0)
		{
			CheckSpawnBoss();
			ResetBossTimer(10.f);
		}
	}

	// check spawn object
	m_objectSpawnTimer -= deltaTime;
	SpawnObjectByProbability();
}

void SpawnerSystem::InitRNGs()
{
	// init a random width generator, with no negative number
	int width = m_pGameRunningState->GetWorld()->GetWidth();
	m_pWidthGenerator = new RandomNumberGenerator(0.f, (float)width, IntOrFloat::kFloat);

	// Init a random spawning probability generator
	m_pSpawningProbabilityRNG = new RandomNumberGenerator(0.f, (float)m_maxProbability);

	// Init spawn object timer RNG
	m_pSpawningTimerRNG = new RandomNumberGenerator(0.5f, 2.f, IntOrFloat::kFloat);

	// Init Zone Type RNG
	m_pZoneAndItemTypeRNG = new RandomNumberGenerator(0.f, 1.f);

	// Init Enemy and Item Type RNG
	m_pEnemyTypeRNG = new RandomNumberGenerator(0.f, 2.f);
}

//-----------------------------------
// Calculate Max Spawning Probability
//-----------------------------------
void SpawnerSystem::CalculateMaxProbability()
{
	for (int i = 0; i < (int)SpawningType::kNumberOfType; ++i)
	{
		m_maxProbability += s_objectSpawningProbability[i].first;
	}
}

void SpawnerSystem::CheckSpawnEnemy()
{
	if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 100.f)
	{
		const int worldWidth = m_pGameRunningState->GetWorld()->GetWidth();
		SDL_Renderer* pRenderer = m_pGameRunningState->GetWorld()->GetRenderer();
		EventSystem* pESystem = m_pGameRunningState->GetWorld()->GetEventSystem();

		// store enemy filename
		const char* enemyImages[3] = { "Assets/EnemyAnimation/Dragon/dragon.png"
										, "Assets/EnemyAnimation/Tree/tree.png"
										, "Assets/EnemyAnimation/Zombie/zombie.png" };

		// randomly selete spawn enemy type
		int type = m_pEnemyTypeRNG->GenerateRandomInt();

		// set y
		float y = -100.f;
		// set random X coordinate
		float randomWidth = m_pWidthGenerator->GenerateRandomFloat();
		if (randomWidth > 100.f)
			randomWidth -= 100.f;

		// create animation sprite shee and animation component
		TextureSpriteSheet* pSpriteSheet = nullptr;
		m_pGameRunningState->GetWorld()->GetImageLoadingSystem()->Load(pRenderer, enemyImages[type], pSpriteSheet);

		// Spawn enemy
		switch (type)
		{
		case 0:
		{
			Dragon* pTemp = SpawnObject<Dragon>(randomWidth, y, pESystem, pSpriteSheet);
			// emplace back to object list
			m_pGameRunningState->AddObject(pTemp);
			break;
		}
		case 1:
		{
			Tree* pTemp = SpawnObject<Tree>(randomWidth, y, pESystem, pSpriteSheet);
			// emplace back to object list
			m_pGameRunningState->AddObject(pTemp);
			break;
		}
		case 2:
		{
			Zombie* pTemp = SpawnObject<Zombie>(randomWidth, y, pESystem, pSpriteSheet);
			// emplace back to object list
			m_pGameRunningState->AddObject(pTemp);
			break;
		}
		default:
			break;
		}
	}
}

void SpawnerSystem::CheckSpawnWall()
{
	if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 600.f)
	{
		const int worldWidth = m_pGameRunningState->GetWorld()->GetWidth();
		SDL_Renderer* pRenderer = m_pGameRunningState->GetWorld()->GetRenderer();

		// get sprite sheet
		TextureSpriteSheet* pSpriteSheet = nullptr;
		m_pGameRunningState->GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/wall.png", pSpriteSheet);
		
		// get y coordinate
		float y = -56.f;

		// set random x
		float randomWidth = m_pWidthGenerator->GenerateRandomFloat();
		if (randomWidth > 200.f)
			randomWidth -= 200.f;

		// create wall and store in object list
		Wall* pTemp = SpawnObject<Wall>(randomWidth, y, pSpriteSheet);

		// emplace back to object list
		m_pGameRunningState->AddObject(pTemp);
	}
}

void SpawnerSystem::CheckSpawnZone()
{
	if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 600.f)
	{
		const int worldWidth = m_pGameRunningState->GetWorld()->GetWidth();
		SDL_Renderer* pRenderer = m_pGameRunningState->GetWorld()->GetRenderer();

		// store zone filename
		const char* zoneImages[2] = { "Assets/ZoneAnimation/damageZone1.png", "Assets/ZoneAnimation/healingZone.png" };

		int zoneType = m_pZoneAndItemTypeRNG->GenerateRandomInt();

		// get sprite sheet
		TextureSpriteSheet* pSpriteSheet = nullptr;
		m_pGameRunningState->GetWorld()->GetImageLoadingSystem()->Load(pRenderer, zoneImages[zoneType], pSpriteSheet);

		// Spawn zone
		switch (zoneType)
		{
		case 0:
		{
			// get y coordinate
			float y = -400.f;
			// set random X coordinate
			float randomWidth = m_pWidthGenerator->GenerateRandomFloat();
			if (randomWidth > 410.f)
				randomWidth -= 400.f;

			// call spawn function
			DamageZone* pTemp = SpawnObject<DamageZone>(randomWidth, y, pSpriteSheet);
			// emplace back to object list
			m_pGameRunningState->AddObject(pTemp);

			break;
		}
		case 1:
		{
			// get y coordinate
			float y = -300.f;
			// set random X coordinate
			float randomWidth = m_pWidthGenerator->GenerateRandomFloat();
			if (randomWidth > 310.f)
				randomWidth -= 300.f;

			// call spawn function
			HealingZone* pTemp = SpawnObject<HealingZone>(randomWidth, y, pSpriteSheet);
			// emplace back to object list
			m_pGameRunningState->AddObject(pTemp);

			break;
		}
		default:
			break;
		}
	}
}

void SpawnerSystem::CheckSpawnItem()
{
	if (m_pGameRunningState->GetBackground()->GetCameraDimention().y > 100.f)
	{
		const int worldWidth = m_pGameRunningState->GetWorld()->GetWidth();
		SDL_Renderer* pRenderer = m_pGameRunningState->GetWorld()->GetRenderer();
		EventSystem* pESystem = m_pGameRunningState->GetWorld()->GetEventSystem();

		// store enemy filename
		const char* itemImages[2] = { "Assets/Item/clearScreen.png"
										, "Assets/Item/treasure.png" };

		// randomly selete spawn enemy type
		int type = m_pZoneAndItemTypeRNG->GenerateRandomInt();

		// set y
		float y = -50.f;
		// set random X coordinate
		float randomWidth = m_pWidthGenerator->GenerateRandomFloat();
		if (randomWidth > 100.f)
			randomWidth -= 100.f;

		// create animation sprite shee and animation component
		TextureSpriteSheet* pSpriteSheet = nullptr;
		m_pGameRunningState->GetWorld()->GetImageLoadingSystem()->Load(pRenderer, itemImages[type], pSpriteSheet);

		// Spawn enemy
		switch (type)
		{
		case 0:
		{
			SuperBomb* pTemp = SpawnObject<SuperBomb>(randomWidth, y, pSpriteSheet);
			// emplace back to object list
			m_pGameRunningState->AddObject(pTemp);
			break;
		}
		case 1:
		{
			WeaponPowerUp* pTemp = SpawnObject<WeaponPowerUp>(randomWidth, y, pSpriteSheet);
			// emplace back to object list
			m_pGameRunningState->AddObject(pTemp);
			break;
		}
		default:
			break;
		}
	}
}

void SpawnerSystem::CheckSpawnBoss()
{
	if (!m_pGameRunningState->GetBoss())
	{
		const int worldWidth = m_pGameRunningState->GetWorld()->GetWidth();
		SDL_Renderer* pRenderer = m_pGameRunningState->GetWorld()->GetRenderer();
		EventSystem* pESystem = m_pGameRunningState->GetWorld()->GetEventSystem();

		// get sprite sheet
		TextureSpriteSheet* pSpriteSheet = nullptr;
		m_pGameRunningState->GetWorld()->GetImageLoadingSystem()->Load(pRenderer, "Assets/Boss/boss.png", pSpriteSheet);

		// get y coordinate
		float y = -350.f;

		// set random x
		float randomWidth = m_pWidthGenerator->GenerateRandomFloat();
		if (randomWidth > 350.f)
			randomWidth -= 350.f;

		// create wall and store in object list
		EnemyBoss* pBoss = SpawnObject<EnemyBoss>(randomWidth, y, pESystem, pSpriteSheet);

		// assign the boss to the m_pBoss as a reference in game running state
		m_pGameRunningState->SetBoss(pBoss);
	}
	else
	{
		// get y coordinate
		float y = -350.f;

		// set random x
		float randomWidth = m_pWidthGenerator->GenerateRandomFloat();
		if (randomWidth > 350.f)
			randomWidth -= 350.f;

		EnemyBoss* pBoss = m_pGameRunningState->GetBoss();

		// reset enemy properties
		pBoss->GetCoordinateVector().m_x = randomWidth;
		pBoss->GetCoordinateVector().m_y = y;
		pBoss->ResetProperty();
		pBoss->Validate();
	}
}

LevelOneCombo* SpawnerSystem::InitLevelOneWeapon()
{
	TextureSpriteSheet* pSpriteSheet = nullptr;
	SimpleSDLGame* pWorld = m_pGameRunningState->GetWorld();
	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), "Assets/Weapon/projectile.jpg", pSpriteSheet);

	// Spawn Projectile
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = { m_pGameRunningState->GetPlayer()->GetX() + 25, m_pGameRunningState->GetPlayer()->GetY() - 21 };
	objectSpawnInfo.m_pSpriteSheet = pSpriteSheet;
	objectSpawnInfo.m_pCollisionManager = pWorld->GetCollisionManager();
	objectSpawnInfo.m_pGameRunningState = m_pGameRunningState;

	Projectile* pProjectile = new Projectile(objectSpawnInfo);

	return new LevelOneCombo( pProjectile );
}

LevelTwoCombo* SpawnerSystem::InitLevelTwoWeapon()
{
	TextureSpriteSheet* pSpriteSheet = nullptr;
	SimpleSDLGame* pWorld = m_pGameRunningState->GetWorld();

	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), "Assets/Weapon/MagicWave.png", pSpriteSheet);

	// Spawn Projectile
	ObjectSpawnInfo objectSpawnInfo;
	objectSpawnInfo.m_spawnLocation = { m_pGameRunningState->GetPlayer()->GetX(), m_pGameRunningState->GetPlayer()->GetY() - 21 };
	objectSpawnInfo.m_pSpriteSheet = pSpriteSheet;
	objectSpawnInfo.m_pCollisionManager = pWorld->GetCollisionManager();
	objectSpawnInfo.m_pGameRunningState = m_pGameRunningState;

	MagicWave* pWave = new MagicWave(objectSpawnInfo);

	return new LevelTwoCombo(pWave);
}

LevelThreeCombo* SpawnerSystem::InitLevelThreeWeapon()
{
	TextureSpriteSheet* pSpriteSheet = nullptr;
	SimpleSDLGame* pWorld = m_pGameRunningState->GetWorld();
	// Spawn Projectile
	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), "Assets/Weapon/MagicWave.png", pSpriteSheet);

	ObjectSpawnInfo spawnInfoOne;
	spawnInfoOne.m_spawnLocation = { m_pGameRunningState->GetPlayer()->GetX(), m_pGameRunningState->GetPlayer()->GetY() - 21 };
	spawnInfoOne.m_pSpriteSheet = pSpriteSheet;
	spawnInfoOne.m_pCollisionManager = pWorld->GetCollisionManager();
	spawnInfoOne.m_pGameRunningState = m_pGameRunningState;

	MagicWave* pWave = new MagicWave(spawnInfoOne);
	pSpriteSheet = nullptr;

	// Spawn Projectile
	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), "Assets/Weapon/SmallWaveRed.png", pSpriteSheet);

	ObjectSpawnInfo spawnInfoTwo;
	spawnInfoTwo.m_spawnLocation = { m_pGameRunningState->GetPlayer()->GetX(), m_pGameRunningState->GetPlayer()->GetY() - 21 };
	spawnInfoTwo.m_pSpriteSheet = pSpriteSheet;
	spawnInfoTwo.m_pCollisionManager = pWorld->GetCollisionManager();
	spawnInfoTwo.m_pGameRunningState = m_pGameRunningState;

	Vector2D shootLeft(-0.8f, -1.f);
	shootLeft = shootLeft.NormalizeVector(0);
	shootLeft.Rotate(shootLeft.m_x, shootLeft.m_y);

	SmallOpticalWave* pTempOne = new SmallOpticalWave(spawnInfoTwo, shootLeft);
	pSpriteSheet = nullptr;

	// Spawn Projectile
	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), "Assets/Weapon/SmallWaveRed.png", pSpriteSheet);

	ObjectSpawnInfo spawnInfoThree;
	spawnInfoThree.m_spawnLocation = { m_pGameRunningState->GetPlayer()->GetX(), m_pGameRunningState->GetPlayer()->GetY() - 21 };
	spawnInfoThree.m_pSpriteSheet = pSpriteSheet;
	spawnInfoThree.m_pCollisionManager = pWorld->GetCollisionManager();
	spawnInfoThree.m_pGameRunningState = m_pGameRunningState;

	Vector2D shootRight(0.8f, -1.f);
	shootRight = shootRight.NormalizeVector(0);
	shootRight.Rotate(shootRight.m_x, shootRight.m_y);

	SmallOpticalWave* pTempTwo = new SmallOpticalWave(spawnInfoThree, shootRight);
	pSpriteSheet = nullptr;

	return new LevelThreeCombo(pWave, pTempOne, pTempTwo);
}

void SpawnerSystem::CheckWeaponComboInit()
{
	switch (m_pGameRunningState->GetPlayer()->GetWeaponLevel())
	{
	case 1:
	{
		m_pGameRunningState->ClearCurrentBullets();
		for (int i = 0; i < 10; ++i)
		{
			LevelOneCombo* combo = InitLevelOneWeapon();
			m_pGameRunningState->GetWeaponVector().emplace_back(combo);
		}
	}
		break;
	case 2:
	{
		m_pGameRunningState->ClearCurrentBullets();
		for (int i = 0; i < 10; ++i)
		{
			LevelTwoCombo* combo = InitLevelTwoWeapon();
			m_pGameRunningState->GetWeaponVector().emplace_back(combo);
		}
	}
		break;
	case 3:
	{
		m_pGameRunningState->ClearCurrentBullets();
		for (int i = 0; i < 10; ++i)
		{
			LevelThreeCombo* combo = InitLevelThreeWeapon();
			m_pGameRunningState->GetWeaponVector().emplace_back(combo);
		}
	}
		break;
	default:
		break;
	}
}