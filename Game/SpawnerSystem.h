#pragma once
#include <memory>
#include "WeaponCombo.h"

class Player;
class GameRunningState;
class EventSystem;
class RandomNumberGenerator;
class Vector2D;
class CollisionComponent;
struct TextureSpriteSheet;
struct SDL_Renderer;

class SpawnerSystem
{
	// determine how many types of objects we could spawn
	enum class SpawningType
	{
		kEnemy = 0,
		kWall,
		kZone,
		kItem,
		kNumberOfType,
	};

	typedef std::pair<int, SpawningType> SpawningTypeProbability;
	// create a probability chart for spawner to check spawning type everytime
	static const SpawningTypeProbability s_objectSpawningProbability[(int)SpawningType::kNumberOfType];

	float m_objectSpawnTimer;							// timer that couns for enemy spawn duration
	float m_bossShowingTimer;							// timer that counts for boss spawn

	int m_maxProbability;								// Probability chart that decides which type of object to spawn

	GameRunningState* m_pGameRunningState;				// Game running state reference
	RandomNumberGenerator* m_pWidthGenerator;			// Generate random float
	RandomNumberGenerator* m_pSpawningProbabilityRNG;	// Spawning Probability RNG
	RandomNumberGenerator* m_pSpawningTimerRNG;			// Spawning Timer RNG
	RandomNumberGenerator* m_pZoneAndItemTypeRNG;		// Zone and Item RNG
	RandomNumberGenerator* m_pEnemyTypeRNG;				// Enemy RNG

public:
	SpawnerSystem(GameRunningState* pGameRunningState);
	~SpawnerSystem();

	// update function to spawn and count timer
	void Update(float deltaTime);

	void CheckSpawnBoss();

	void CheckWeaponComboInit();

private:
	// Setter for Spawn Timer 
	void SetObjectSpawnTimer(float time) { m_objectSpawnTimer = time; };

	// template spawning function that responsible for spawning all object ( function overload)
	template<class SpawnType>
	SpawnType* SpawnObject(float x, float y, TextureSpriteSheet* pSpriteSheet);
	template<class SpawnType>
	SpawnType* SpawnObject(float x, float y, Vector2D direction, TextureSpriteSheet* pSpriteSheet);
	template<class SpawnType>
	SpawnType* SpawnObject(float x, float y, EventSystem* pEventSystem, TextureSpriteSheet* pSpriteSheet);


	// Init RNGs
	void InitRNGs();

	// Calculate max probability
	void CalculateMaxProbability();

	// Probability Spawn Objects
	void SpawnObjectByProbability();

	// check spawn functions
	void CheckSpawnEnemy();
	void CheckSpawnWall();
	void CheckSpawnZone();
	void CheckSpawnItem();

	// reset boss showing timer
	void ResetBossTimer(float time) { m_bossShowingTimer = time; }

	// Init Player weapon combo
	LevelOneCombo* InitLevelOneWeapon();
	LevelTwoCombo* InitLevelTwoWeapon();
	LevelThreeCombo* InitLevelThreeWeapon();
};
