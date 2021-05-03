#pragma once
#include <stdint.h>

class ObjectBase;

class Event
{
public:
	enum EventType
	{
		kMoveDown = 0,
		kMoveUp,
		kMoveLeft,
		kMoveRight,
		kChase,
		kAttact,
		kGetHurt,
		kHeal,
		kNumOfEvent,
	};

private:
	ObjectBase* m_pEventOwner;
	EventType m_type;

public:
	Event(ObjectBase* pEventOwner, EventType type);
	~Event();

	ObjectBase* GetEventOwner() { return m_pEventOwner; };
	EventType GetEventType() { return m_type; };
};

