#include "UIButton.h"
#include "AnimationComponent.h"
//-----------------------------
//	Constructor
//-----------------------------
UIButton::UIButton(float x, float y, float w, float h, AnimationComponent* pAnim)
	: m_transform{x,y,w,h}
	, m_callback{nullptr}
	, m_pAnim{ pAnim }
	, m_isHighlighted{false}
{
	//
}

//-----------------------------
//	Destructor
//-----------------------------
UIButton::~UIButton()
{
	delete m_pAnim;
	m_pAnim = nullptr;
}

//--------------------------------------------
//	Set Button Animation when not highlighted
//--------------------------------------------
void UIButton::SetAnimationNotHighlighted(int startFrame, int endFrame)
{
	m_pAnim->AddAnimationSequence("Not Highlighted", startFrame, endFrame);
}

//--------------------------------------------
//	Set Button Animation when highlighted
//--------------------------------------------
void UIButton::SetAnimationHighlighted(int startFrame, int endFrame)
{
	m_pAnim->AddAnimationSequence("Highlighted", startFrame, endFrame);
}

//------------------------------------
//	Reposition button at new location
//------------------------------------
void UIButton::ButtonMoving(float deltaX, float deltaY)
{
	m_transform.x += deltaX;
	m_transform.y += deltaY;
}

//-----------------------------
//	Update Button Anim
//-----------------------------
void UIButton::Update(float deltaTime)
{
	if(m_isHighlighted)
		m_pAnim->PlayAnimation("Highlighted");
	else
		m_pAnim->PlayAnimation("Not Highlighted");

	m_pAnim->Update(deltaTime);
}

//-----------------------------
//	Render the Button
//-----------------------------
void UIButton::Render(SDL_Renderer* pRenderer)
{
	m_pAnim->Render(pRenderer, m_transform.x, m_transform.y, m_transform.w, m_transform.h, 0);
}

//-----------------------------
//	Trigger callback when click
//-----------------------------
void UIButton::Trigger()
{
	if (m_callback != nullptr)
		m_callback();
}

//-------------------------------------
//	Test if mouse is within button area
//-------------------------------------
bool UIButton::HitTest(int x, int y)
{
	return ((float)x > m_transform.x && (float)x < m_transform.x + m_transform.w &&
		(float)y > m_transform.y && (float)y < m_transform.y + m_transform.h);
}
