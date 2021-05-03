#include "StaticBoarder.h"
#include "GameRunningState.h"
StaticBoarder::StaticBoarder(GameRunningState* pRunningState, float x, float y)
{
	m_pGameRunningState = pRunningState;
	// set wall dimention
	m_coordinates = { x,y };
	m_dimentions = { 3, 3942 };
	m_isValid = true;
	m_collisionBox = { (int)x , (int)y, (int)m_dimentions.m_x, (int)m_dimentions.m_y };

	m_objectType = ObjectBase::ObjectType::kBoarder;
}

void StaticBoarder::Render(SDL_Renderer* pRenderer)
{
}

void StaticBoarder::Tick(float deltaTime)
{
	//
}

void StaticBoarder::HandleBeginOverlap(ObjectBase* pOtherCollider)
{
	if (pOtherCollider->IsValid() && m_isValid)
	{
		if (pOtherCollider->GetX() > m_coordinates.m_x)
		{
			pOtherCollider->GetCoordinateVector().m_x = m_coordinates.m_x + m_dimentions.m_x;
		}

		if (pOtherCollider->GetX() < m_coordinates.m_x)
		{
			pOtherCollider->GetCoordinateVector().m_x = m_coordinates.m_x - pOtherCollider->GetWidth();
		}
	}
}
