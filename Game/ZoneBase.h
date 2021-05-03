#pragma once
#include "ObjectBase.h"
class ZoneBase : public ObjectBase
{
protected:
	ZoneBase() :m_effectTimer(1.f) {};

	float m_effectTimer;
public:
	~ZoneBase() {};
};

