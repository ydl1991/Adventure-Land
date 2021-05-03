#pragma once
#include "SDL_rect.h"
#include "SDL_ttf.h"
#include <string>
#include <functional>
class EventSystem;
class SimpleSDLGame;
class UITextField;
class UIButton;
class UITextureButton;
class ReadWriteSystem;
struct TextureSpriteSheet;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Surface;

class UISavingLoading
{
public:
	enum class Menu { kOff, kEntry, kSave, kLoad };

private:
	// game world reference
	SimpleSDLGame* m_pGameWorld;
	ReadWriteSystem* m_pReadWriteSystem;

	// Game Screenshot at point save
	SDL_Surface* m_pCurrentSurface;
	static constexpr int m_kNumOfSaveFiles = 3;
	const char* m_kFileName[m_kNumOfSaveFiles] = { "Screenshot1.bmp", "Screenshot2.bmp", "Screenshot3.bmp" };
	static constexpr char s_kDirectory[] = "saves\\";

	// Entry menu Box texture and location
	SDL_Texture* m_pInputBoxTexture;
	SDL_Rect m_boarderBoxRect;

	// Entry menu Title textfield
	UITextField* m_pTitleTextField;

	// Entry menu buttons
	UIButton* m_pSaveButton;
	UITextField* m_pSaveTextField;
	UIButton* m_pLoadButton;
	UITextField* m_pLoadTextField;

	// Selection menu Box texture and location
	SDL_Texture* m_pSelectionBoxTexture;
	SDL_Rect m_selectionBoxRect;

	// Selection menu Game spot 1
	UITextureButton* m_pGameSlotOne;
	UITextField* m_pGameOneSaveTime;

	// Selection menu Game spot 2
	UITextureButton* m_pGameSlotTwo;
	UITextField* m_pGameTwoSaveTime;

	// Selection menu Game spot 3
	UITextureButton* m_pGameSlotThree;
	UITextField* m_pGameThreeSaveTime;

	// menu enum class
	Menu m_menu;

	// flags to determine updated screenshot
	bool m_isSlotOneDirty;
	bool m_isSlotTwoDirty;
	bool m_isSlotThreeDirty;

public:
	UISavingLoading(SimpleSDLGame* pGameWorld);
	~UISavingLoading();

	// update and render
	void Update(float deltaTime);
	void Render(SDL_Renderer* pRenderer);

	// set rects
	void SetBoarderRect(int x, int y, int w, int h);

	// Set Current Menu
	void SetCurrentMenu(Menu menu) { m_menu = menu; }
	Menu GetCurrentMenu() const { return m_menu; }

	// Take Game Screenshot
	void TakeGameScreenshot();
	void SaveScreenshotToFile(int slot);
	void FreeScreenshot();

	// Get Buttons
	UIButton* GetSaveButton() const { return m_pSaveButton; }
	UIButton* GetLoadButton() const { return m_pLoadButton; }
	UITextureButton* GetGameOneSlot() const { return m_pGameSlotOne; }
	UITextureButton* GetGameTwoSlot() const { return m_pGameSlotTwo; }
	UITextureButton* GetGameThreeSlot() const { return m_pGameSlotThree; }

	// Get World
	SimpleSDLGame* GetWorld() const { return m_pGameWorld; }

private:
	// Init
	void Init();

	// Init Save Button
	void InitSaveButton(SDL_Renderer* pRenderer, TextureSpriteSheet* pSpriteSheet, TTF_Font* pFont, SDL_Color color);
	void InitLoadButton(SDL_Renderer* pRenderer, TextureSpriteSheet* pSpriteSheet, TTF_Font* pFont, SDL_Color color);

	// Init Game slots
	void InitGameSlotOne(SDL_Renderer* pRenderer, SDL_Texture* pTexture, TTF_Font* pFont, SDL_Color color);
	void InitGameSlotTwo(SDL_Renderer* pRenderer, SDL_Texture* pTexture, TTF_Font* pFont, SDL_Color color);
	void InitGameSlotThree(SDL_Renderer* pRenderer, SDL_Texture* pTexture, TTF_Font* pFont, SDL_Color color);

	// Update Saved Screenshot
	void UpdateSavedScreenShot();
};

