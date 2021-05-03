#include "UIPlayerInfo.h"
#include "Player.h"
#include "UITextField.h"
#include "SDL_ttf.h"

UIPlayerInfo::UIPlayerInfo(Player* pPlayer, SDL_Texture* pPlayerIcon, SDL_Renderer* pRenderer)
	: m_IconRect{0,0, 60, 60}
	, m_pPlayerIcon{ pPlayerIcon }
	, m_pPlayer{ pPlayer }
	, m_pPlayerNameDisplay{ nullptr }
{
	Init(pRenderer);
}

UIPlayerInfo::~UIPlayerInfo()
{
	delete m_pPlayerNameDisplay;
	m_pPlayerNameDisplay = nullptr;
}

void UIPlayerInfo::Render(SDL_Renderer* pRenderer)
{
	SDL_RenderCopy(pRenderer, m_pPlayerIcon, nullptr, &m_IconRect);
	m_pPlayerNameDisplay->Render(pRenderer);
}

void UIPlayerInfo::SetIconRect(int x, int y)
{
	m_IconRect.x = x;
	m_IconRect.y = y;
}

void UIPlayerInfo::Init(SDL_Renderer* pRenderer)
{
	TTF_Font* pFont = TTF_OpenFont("Fonts/Teko-SemiBold.ttf", 40);
	if (!pFont)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}
	//TTF_SetFontStyle(pFont, TTF_STYLE_BOLD);
	SDL_Color nameColor = { 255, 153, 0, 255 };
	m_pPlayerNameDisplay = new UITextField(80, 12, pFont, m_pPlayer->GetName().c_str(), nameColor, pRenderer);
}
