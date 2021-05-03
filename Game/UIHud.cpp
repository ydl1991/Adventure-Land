#include "UIHud.h"
#include "GameRunningState.h"
#include "SDL.h"
#include "SDL_image.h"
#include "ImageLoadingSystem.h"
#include "SimpleSDLGame.h"
#include "UITextField.h"
#include "Player.h"
#include "SDL_ttf.h"
#include "UIPlayerInfo.h"
#include "UIHealth.h"

//-----------------------------
//	Constructor
//-----------------------------
UIHud::UIHud(GameRunningState* pGameRunningState, int playerType)
	: m_pGameRunningState{ pGameRunningState }
	, m_pObjectHealthUI{nullptr}
	, m_pPlayerInfo{nullptr}
	, m_pHudLayout{nullptr}
	, m_hudRect{0,0,250,120}
{
	InitHudLayout();
	InitUIPlayerInfo(playerType);
	InitUIHealth();
}

//-----------------------------
//	Destructor
//-----------------------------
UIHud::~UIHud()
{
	delete m_pObjectHealthUI;
	m_pObjectHealthUI = nullptr;
	delete m_pPlayerInfo;
	m_pPlayerInfo = nullptr;
}

//-----------------------------
//	Update HUD Info
//-----------------------------
void UIHud::Update(float deltaTime)
{
	m_pObjectHealthUI->Update(deltaTime);
}

//-----------------------------
//	Render HUD
//-----------------------------
void UIHud::Render(SDL_Renderer* pRenderer)
{
	SDL_RenderCopy(pRenderer, m_pHudLayout, nullptr, &m_hudRect);
	m_pPlayerInfo->Render(pRenderer);
	m_pObjectHealthUI->Render(pRenderer);
}

void UIHud::SetLayoutRect(int x, int y)
{
	m_hudRect.x = x;
	m_hudRect.y = y;
}

//-----------------------------
//	Init Health UI
//-----------------------------
void UIHud::InitUIHealth()
{
	SimpleSDLGame* pWorld = m_pGameRunningState->GetWorld();
	SDL_Texture* pTexture = nullptr;
	// load texture for icon
	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), "Assets/healthIcon.png", pTexture);
	// create the Health UI
	m_pObjectHealthUI = new UIHealth(m_pGameRunningState->GetPlayer(), pTexture);
	m_pObjectHealthUI->SetIconRect(20, 65);
	m_pObjectHealthUI->SetBaseRect(80, 75);
	m_pObjectHealthUI->SetHealthRect(80,75);
}

//-----------------------------
//	Init Text Field Display
//-----------------------------
void UIHud::InitUIPlayerInfo(int playerType)
{
	const char* playerProfile[2] = { "Assets/playerprofile.png", "Assets/warplaneprofile.png" };

	SDL_Texture* pTexture = nullptr;
	SimpleSDLGame* pWorld = m_pGameRunningState->GetWorld();
	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), playerProfile[playerType], pTexture);
	m_pPlayerInfo = new UIPlayerInfo(m_pGameRunningState->GetPlayer(), pTexture, pWorld->GetRenderer());
	m_pPlayerInfo->SetIconRect(12,7);
}

void UIHud::InitHudLayout()
{
	SimpleSDLGame* pWorld = m_pGameRunningState->GetWorld();
	pWorld->GetImageLoadingSystem()->Load(pWorld->GetRenderer(), "Assets/HUD.png", m_pHudLayout);
}
