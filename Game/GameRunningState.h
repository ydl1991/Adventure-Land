#pragma once
#include "GameState.h"
#include "EventListener.h"
#include "WeaponCombo.h"
#include <list>
#include <functional>
#include <string>

class SimpleSDLGame;
class StateMachine;
class ImageLoadingSystem;
class AnimationComponent;
class CollisionManager;
class EventSystem;
class ObjectBase;
class Player;
class Projectile;
class Enemy;
class EnemyBoss;
class Wall;
class ZoneBase;
class WorldBackground;
class SpawnerSystem;
class UIHud;
class SoundPlayer;
class FireBall;
class WinZone;

/***********************************************************/
// Game Running state that processes all objects update and 
// events
/***********************************************************/
class GameRunningState : public GameState, public EventListener
{
private:
	//-------------------------------------//
	//			Private Variables
	//-------------------------------------//
	std::function<void()> m_winCallback;											// A callback that determines win
	std::function<void()> m_loseCallback;											// A callback that determines lose
	std::list<ObjectBase*> m_allObjects;											// A list that stores all objects
	std::vector<FireBall*> m_fireBalls;												// A vector of FireBalls
	std::vector<WeaponCombo*> m_WeaponCompo;											// A vector of player's weapon

	Player* m_pPlayer;																// A reference of the player
	EnemyBoss* m_pBoss;																// A reference of the boss
	WinZone* m_pWinZone;															// A reference of the win zone

	WorldBackground* m_pWorldBackground;											// A reference of the game background
	SpawnerSystem* m_pSpawnerSystem;												// A reference of the spawner system
	StateMachine* m_pStateMachine;													// A reference of the State Machine
	SoundPlayer* m_pSoundPlayer;													// A sound player that uses to play game music and sound effects
	UIHud* m_pHud;																	// A user interface that display player name and health

	// Traits for fire balls
	float m_circleParameters[14];													// parameters assist in drawing a circle around boss

	float m_fireTimer;																// timer that counts for projectile spawn duration
	size_t m_fireComboIndicator;													// Weapon fire indicator

	// Quit Game Conditions
	bool m_pressCtrl;
	bool m_pressQ;

public:
	int m_playerType;																// which player character 

public:
	GameRunningState(SimpleSDLGame* pGameWorld, int playerType, std::string pName);
	~GameRunningState();

	//-------------------------------------//
	//			Public Functions
	//-------------------------------------//
	// virtual function to override
	virtual void EnterState() override;												// Init the game state
	virtual void Update(float deltaTime) override;									// Update game objects under current game state
	virtual void ExitState() override;												// Wrap up everything before game state gets deleted 
	virtual bool ProcessEvents() override;											// Process event under current game state
	virtual void RenderCurrentState(SDL_Renderer* pRenderer) override;				// Render current game state
	void ProcessInputEvent(SDL_Event& evt) override;								// Process Input Event

	void AddObject(ObjectBase* pObject);											// Add new object to object list

	// Getters
	Player* GetPlayer() const { return m_pPlayer; };								// Get the player reference
	EnemyBoss* GetBoss() const { return m_pBoss; };									// Get the boss reference
	std::list<ObjectBase*>& GetObjectList() { return m_allObjects; };				// Get the object list reference
	std::vector<FireBall*>& GetFireBalls() { return m_fireBalls; }
	WorldBackground* GetBackground() const { return m_pWorldBackground; };			// Get the background under current game state
	SpawnerSystem* GetSpawnerSystem() const { return m_pSpawnerSystem; };			// Get the object spawner
	SoundPlayer* GetSoundPlayer() const { return m_pSoundPlayer; };					// Get the sound player
	std::vector<WeaponCombo*>& GetWeaponVector() { return m_WeaponCompo; }
	//size_t GetWeaponVectorSize() const { return ; }

	// Setter
	void SetBoss(EnemyBoss* pBoss) { m_pBoss = pBoss; }

	// Load Game Preparation
	void LoadGamePreparation();

	// Explosion Item Effect
	void ClearOutScreen();

	// clear current bullet
	void ClearCurrentBullets();

private:
	//-------------------------------------//
	//			Private Functions
	//-------------------------------------//
	// Cleanning
	void RemoveInvalidObject();														// remove object that becomes invalid
	void ClearAll();																// clear up all memory
	void InternalClearCurrentBullets();

	// object initialization functions
	void CreatePlayer(int playerType, std::string pName);
	void CreateBackground();
	void CreateWinZone();
	void CreateBoarder();
	void CreateHUD(int playerType);
	void LoadBackgroundSounds();

	// crete boss and init boss fireballs
	void CreateFireBalls();
	void InitFireballTrace();

	// updates
	void UpdateObjectList(float deltaTime);
	void UpdateFireBalls(float deltaTime);
	void UpdateWeaponCombo(float deltaTime);
	void CheckFireProjectile(float deltaTime);

	// renders
	void RenderObjectList(SDL_Renderer* pRenderer);
	void RenderFireballs(SDL_Renderer* pRenderer);
	void RenderBullets(SDL_Renderer* pRenderer);

	// Game State Action Event
	void ProcessPressKeyBoardEvent(SDL_KeyboardEvent* pData);
	void ProcessReleaseKeyBoardEvent(SDL_KeyboardEvent* pData);
	void ProcessMouseClickEvent(SDL_MouseButtonEvent* pData);
	void ProcessWindowEvent(SDL_WindowEvent* pData);
};

