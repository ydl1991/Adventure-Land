#include "UITextureButton.h"
#include "SDL.h"

UITextureButton::UITextureButton(float x, float y, float w, float h)
	: m_transform{ x,y,w,h }
	, m_callback{ nullptr }
	, m_pTexture{ nullptr }
{
	//
}

UITextureButton::~UITextureButton()
{
}

void UITextureButton::Update(float deltaTime)
{
}

void UITextureButton::Render(SDL_Renderer* pRenderer)
{
	SDL_Rect transform = { (int)m_transform.x, (int)m_transform.y, (int)m_transform.w, (int)m_transform.h };

	if (m_pTexture)
	{
		SDL_RenderCopy(pRenderer, m_pTexture, nullptr, &transform);
	}
	else
	{
		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 50);
		SDL_RenderFillRectF(pRenderer, &m_transform);
	}
}

void UITextureButton::Trigger()
{
	if (m_callback != nullptr)
		m_callback();
}

bool UITextureButton::HitTest(int x, int y)
{
	return ((float)x > m_transform.x && (float)x < m_transform.x + m_transform.w &&
		(float)y > m_transform.y && (float)y < m_transform.y + m_transform.h);
}
