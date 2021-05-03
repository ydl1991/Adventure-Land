#pragma once
#include "Event.h"
#include "EventListener.h"
#include <vector>

struct SDL_WindowEvent;
struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
struct SDL_TextInputEvent;

/***********************************************************/
// EventSystem that processes Normal event and SDL events and 
// take down info that affects the world
/***********************************************************/
class EventSystem
{
	/*******************************************************/
	//					Normal Event Related
	/*******************************************************/
	std::vector<EventListener*> m_enemyEventProcessingList[Event::EventType::kNumOfEvent];

	/*******************************************************/
	//					SDL Event Related
	/*******************************************************/
	std::vector<EventListener*> m_eventListenerList;

	// Game State Transition Flag
	bool m_transitionStarted;

public:
	// Constructor and destructor
	EventSystem();
	~EventSystem();

	/*******************************************************/
	//					Game Runing Event Related
	/*******************************************************/
	// Add event listeners
	void AddNormalEventListener(EventListener* pEventOwner, Event::EventType type);
	void AddInputEventListener(EventListener* pEventOwner);
	// Remove event listeners
	void RemoveInputEventListener(EventListener* pEventOwner);
	void RemoveNormalEventListener(EventListener* pEventOwner);
	// process other object events
	void ProcessNormalEvents();
	// process Input events
	void ProcessSDLInputEvents();
	void ProcessSDLPostGameInputEvents();
	// Set Transition
	void SetTransition(bool transitionFlag) { m_transitionStarted = transitionFlag; }
	// Clear events
	void ClearNormalEvents();
};

