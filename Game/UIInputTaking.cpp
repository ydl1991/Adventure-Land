#include "UIInputTaking.h"
#include "EventSystem.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "UITextField.h"
#include "SimpleSDLGame.h"

UIInputTaking::UIInputTaking(SimpleSDLGame* pGameWorld, const char* pTitle, SDL_Texture* pInputBoxTexture)
	: m_pGameWorld{ pGameWorld }
	, m_pInputBoxTexture{ pInputBoxTexture }
	, m_boarderBoxRect{}
	, m_pInputBoxTitleMessage{ pTitle }
	, m_pTitleTextField{nullptr}
	, m_textRect{}
	, m_textRectColor{ 217,217,217,255 }
	, m_pInputText{" "}
	, m_done{ false }
	, m_quit{ false }
	, m_lastFrameStringLength{0}
{
	//
	InitTitle();
	SDL_StartTextInput();
	SDL_SetTextInputRect(&m_textRect);
}

UIInputTaking::~UIInputTaking()
{
	delete m_pTitleTextField;
	m_pTitleTextField = nullptr;

	delete m_pInputTextField;
	m_pInputTextField = nullptr;

	m_pInputText = "";
}

void UIInputTaking::Update(float deltaTime)
{
	//ProcessInputEvent();
	UpdateInputTextField();
}

void UIInputTaking::Render(SDL_Renderer* pRenderer)
{
	SDL_RenderCopy(pRenderer, m_pInputBoxTexture, nullptr, &m_boarderBoxRect);
	m_pTitleTextField->Render(pRenderer);

	SDL_SetRenderDrawColor(pRenderer, m_textRectColor.r, m_textRectColor.g, m_textRectColor.b, m_textRectColor.a);
	SDL_RenderFillRect(pRenderer, &m_textRect);

	if(m_pInputTextField)
		m_pInputTextField->Render(pRenderer);
}

void UIInputTaking::SetBoarderRect(int x, int y, int w, int h)
{
	m_boarderBoxRect.x = x;
	m_boarderBoxRect.y = y;
	m_boarderBoxRect.w = w;
	m_boarderBoxRect.h = h;
}

void UIInputTaking::SetTextRect(int x, int y, int w, int h)
{
	m_textRect.x = x;
	m_textRect.y = y;
	m_textRect.w = w;
	m_textRect.h = h;
}

void UIInputTaking::InitTitle()
{
	TTF_Font* pFont = TTF_OpenFont("Fonts/Teko-SemiBold.ttf", 35);
	if (!pFont)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}
	SDL_Color white = { 255, 255, 255, 255 };
	m_pTitleTextField = new UITextField(280,240, pFont, m_pInputBoxTitleMessage, white, m_pGameWorld->GetRenderer());
}

void UIInputTaking::UpdateInputTextField()
{
	if (m_pInputText.length() != m_lastFrameStringLength)
	{
		if (m_pInputTextField)
		{
			delete m_pInputTextField;
			m_pInputTextField = nullptr;
		}

		TTF_Font* pFont = TTF_OpenFont("Fonts/Teko-Medium.ttf", 35);
		if (!pFont)
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			return;
		}
		SDL_Color orange = { 255, 153, 0, 255 };
		m_pInputTextField = new UITextField(335, 295, pFont, m_pInputText.c_str(), orange, m_pGameWorld->GetRenderer());
		m_lastFrameStringLength = m_pInputText.length();
	}
}
