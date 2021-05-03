#include "Event.h"
#include "ObjectBase.h"

Event::Event(ObjectBase* pEventOwner, EventType type)
	: m_pEventOwner{ pEventOwner }
	, m_type{ type }
{
	//
}

Event::~Event()
{
	//
}
