#pragma once
#include "SDL_rect.h"
#include <string>
#include <functional>

class EventSystem;
class SimpleSDLGame;
class UITextField;
struct SDL_Texture;
struct SDL_Renderer;

class UIInputTaking
{
	SimpleSDLGame* m_pGameWorld;
	SDL_Texture* m_pInputBoxTexture;
	SDL_Rect m_boarderBoxRect;

	const char* m_pInputBoxTitleMessage;
	UITextField* m_pTitleTextField;

	SDL_Rect m_textRect;
	SDL_Color m_textRectColor;

	std::string m_pInputText;
	UITextField* m_pInputTextField;
	size_t m_lastFrameStringLength;

	bool m_done;
	bool m_quit;

public:
	UIInputTaking(SimpleSDLGame* pGameWorld, const char* pTitle, SDL_Texture* pInputBoxTexture);
	~UIInputTaking();

	// update and render
	void Update(float deltaTime);
	void Render(SDL_Renderer* pRenderer);

	// set rects
	void SetBoarderRect(int x, int y, int w, int h);
	void SetTextRect(int x, int y, int w, int h);
	//void SetMarkedRect(int w, int h);

	// Set finish condtion
	void SetDone(bool done) { m_done = done; }
	bool Done() const { return m_done; }

	// set quit
	void SetQuit(bool quit) { m_quit = quit; }
	bool Quit() const { return m_quit; }

	// Get the input text
	std::string& GetInputTextString() { return m_pInputText; }
	const char* GetInputText() { return m_pInputText.c_str(); }

	// Get World
	SimpleSDLGame* GetWorld() const { return m_pGameWorld; }

private:
	// init text field title
	void InitTitle();
	void UpdateInputTextField();
};

