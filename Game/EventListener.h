#pragma once
#include <SDL.h>
#include "Event.h"
class EventListener
{
public:
	EventListener() {};
	~EventListener() {};

	virtual void ProcessInputEvent(SDL_Event& evt) {};
	virtual void ProcessEnemyEvent(Event::EventType evtType) {};
};