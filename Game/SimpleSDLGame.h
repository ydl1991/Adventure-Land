#pragma once
struct SDL_Renderer;
struct SDL_Window;
class EventSystem;
class ImageLoadingSystem;
class SoundLoadingSystem;
class CollisionManager;
class StateMachine;
class ReadWriteSystem;
class UISavingLoading;
/***********************************************************/
// SimpleSDLGame is the Game World class that contains 
// game rules and perform Game State Update
/***********************************************************/
class SimpleSDLGame
{
private:
	/***************************************/
	// Private member variables declarition
	/***************************************/
	// Window Length
	const int m_kWindowWidth = 800;				// window size variable 
	const int m_kWindowHeight = 600;			// window size variable

	//// Background
	//SDL_FRect m_camaraOnBackground;

	// Systematic Variable
	SDL_Window* m_pWindow;						// window variable
	SDL_Renderer* m_pRenderer;					// renderer variable
	ImageLoadingSystem* m_pImageSystem;			// image system that takes in file name and retrieves texture
	SoundLoadingSystem* m_pSoundSystem;			// sound system that takes in file name and retrives music or chunk
	EventSystem* m_pEventSystem;				// event system that processes SDL keyboard, mouse and window event
	CollisionManager* m_pCollisionManager;		// a system that detects and handles object collision
	StateMachine* m_pStateMachine;				// a system that handles different game states
	ReadWriteSystem* m_pReadWriteSystem;		// a system that handles saving and loading of the game
	UISavingLoading* m_pSavingLoadingMenu;		// A user interface that displays saving and loading menu

public:
	/***************************************/
	// Public member function declarition
	/***************************************/
	// con and destructor
	SimpleSDLGame();
	~SimpleSDLGame();

	void Init();								// initialization function
	void ShutDown();							// kill everything after finish output
	void Update();								// Game Update

	// Getter
	EventSystem* GetEventSystem() const { return m_pEventSystem; };
	SDL_Renderer* GetRenderer() const { return m_pRenderer; };
	CollisionManager* GetCollisionManager() const { return m_pCollisionManager; };
	ImageLoadingSystem* GetImageLoadingSystem() const { return m_pImageSystem; };
	SoundLoadingSystem* GetSoundLoadingSystem() const { return m_pSoundSystem; };
	StateMachine* GetStateMachine() const { return m_pStateMachine; };
	ReadWriteSystem* GetReadWriteSystem() const { return m_pReadWriteSystem; };
	UISavingLoading* GetSavingLoadingMenu() const { return m_pSavingLoadingMenu; }
	const int GetWidth() { return m_kWindowWidth; };
	const int GetHeight() { return m_kWindowHeight; };

private:
	void InitSystems();
	void InitGameMenu();
};

