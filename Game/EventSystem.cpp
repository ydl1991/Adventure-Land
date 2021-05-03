#include "EventSystem.h"
#include <SDL.h>

EventSystem::EventSystem()
	: m_transitionStarted{ false }
{
	//
}

EventSystem::~EventSystem()
{
	//
}

//---------------------------------------------------------------------
// call to add enemy event listener to the event processing vector
//---------------------------------------------------------------------
void EventSystem::AddNormalEventListener(EventListener* pEventOwner, Event::EventType type)
{
	if (pEventOwner)
	{
		m_enemyEventProcessingList[type].emplace_back(pEventOwner);
	}
}

//---------------------------------------------------------------------
// call to add input event listener to the input event processing vector
//---------------------------------------------------------------------
void EventSystem::AddInputEventListener(EventListener* pEventOwner)
{
	if (pEventOwner)
	{
		m_eventListenerList.emplace_back(pEventOwner);
	}
}

//---------------------------------------------------------------------
// Erase listener from the vector when they becomes irrelevant
//---------------------------------------------------------------------
void EventSystem::RemoveInputEventListener(EventListener* pEventOwner)
{
	for (auto it = m_eventListenerList.begin(); it != m_eventListenerList.end(); ++it)
	{
		if ((*it) == pEventOwner)
		{
			it = m_eventListenerList.erase(it);
			break;
		}
	}
}

void EventSystem::RemoveNormalEventListener(EventListener* pEventOwner)
{
	// Loop through list representing each event type
	for (int i = 0; i < Event::EventType::kNumOfEvent; ++i)
	{
		// loop through and process the listeners in that event
		for (auto it = m_enemyEventProcessingList[i].begin(); it != m_enemyEventProcessingList[i].end();)
		{
			if ((*it) == pEventOwner)
				it = m_enemyEventProcessingList[i].erase(it);
			else
				++it;
				
		}
	}
}

//------------------------
// call to process normal event 
//------------------------
void EventSystem::ProcessNormalEvents()
{
	// Loop through list representing each event type
	for (int i = 0; i < Event::EventType::kNumOfEvent; ++i)
	{
		// loop through and process the listeners in that event
		for (int j = 0; j < m_enemyEventProcessingList[i].size(); ++j)
		{
			if(m_enemyEventProcessingList[i][j])
				m_enemyEventProcessingList[i][j]->ProcessEnemyEvent((Event::EventType)i);
		}
		// clear out listener after finish processing that list
		m_enemyEventProcessingList[i].clear();
	}
}

//-------------------------------------------------------------------------------------------
// call to process events including keyboard event, mouse motion and click, and window event
//-------------------------------------------------------------------------------------------
void EventSystem::ProcessSDLInputEvents()
{
	// create event variable
	SDL_Event evt;

	while (SDL_PollEvent(&evt))
	{
		for (int i = 0; i < m_eventListenerList.size(); ++i)
		{
			m_eventListenerList[i]->ProcessInputEvent(evt);
		}
	}
}

void EventSystem::ProcessSDLPostGameInputEvents()
{
	// create event variable
	SDL_Event evt;

	while (SDL_PollEvent(&evt) && !m_transitionStarted)
	{
		for (int i = 0; i < m_eventListenerList.size(); ++i)
		{
			m_eventListenerList[i]->ProcessInputEvent(evt);
		}
	}
}

void EventSystem::ClearNormalEvents()
{
	for (int i = 0; i < Event::EventType::kNumOfEvent; ++i)
	{
		m_enemyEventProcessingList[i].clear();
	}

	m_enemyEventProcessingList->clear();
}
