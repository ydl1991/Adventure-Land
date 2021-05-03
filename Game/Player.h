#pragma once
#include "ObjectBase.h"
#include "EventListener.h"
#include "SDL_rect.h"
#include <string>

class SimpleSDLGame;

/***********************************************************/
// Player is a input-controlled game object that can fire
// projectile, collid with others.
/***********************************************************/
class Player : public ObjectBase, public EventListener
{
	bool m_isWinner;							// detect player win

	// fire related
	bool m_readyToFire;							// detect firing status		
	float m_fireRate;							// fire rate
	int m_weaponLevel;							// weapon power up level

	// Mouse Event Variable
	bool m_mouseControlMotion;					// detect moving status
	bool m_mouseClick;
	bool m_mouseHolding;
	int m_mouseX;
	int m_mouseY;

	std::string m_pName;

public:
	// Constructor and destructor
	Player(ObjectSpawnInfo objectSpawnInfo, std::string pName);
	~Player();

	// Action Related Functions //
	bool GetMotionState() { return m_mouseControlMotion; };
	void ChangeMotionState() { m_mouseControlMotion ? m_mouseControlMotion = false : m_mouseControlMotion = true; };

	// Speed Related Functions //
	void ChangePlayerSpeed() { (m_speed == 250.f) ? m_speed = 500.f : m_speed = 250.f; };

	// Fire Related Functions
	float GetFireRate() const { return m_fireRate; };
	void SetFireRate(float newRate) { m_fireRate = newRate; };
	void FireSwitch() { m_readyToFire = 1 - m_readyToFire; };
	bool GetFireStatus() const { return m_readyToFire; };
	int GetWeaponLevel() const { return m_weaponLevel; }
	void SetWeaponLevel(int weaponLevel) { m_weaponLevel = weaponLevel; }
	void WeaponLevelUp();

	// Get Object Name
	std::string GetName() const { return m_pName; };
	void SetName(std::string name) { m_pName = name; };

	// Win Game!
	bool HasWon() const { return m_isWinner; };
	void Win() { m_isWinner = true; };

	// virtual override
	void Init() override;
	void Init(int type);
	void Tick(float deltaTime) override;
	void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	void Render(SDL_Renderer* pRenderer) override;
	void ProcessInputEvent(SDL_Event& evt) override;

private:
	void CalculateRotation();														// rotation angle calculation
	void MotionChange(float deltaTime, SimpleSDLGame* pGameWorld);					// moving action

	// player action event
	void ProcessPressKeyBoardEvent(SDL_KeyboardEvent* pData);
	void ProcessReleaseKeyBoardEvent(SDL_KeyboardEvent* pData);
	void ProcessMouseClickEvent(SDL_MouseButtonEvent* pData);
	void ProcessMouseReleaseEvent(SDL_MouseButtonEvent* pData);
	void ProcessMouseMotionEvent(SDL_MouseMotionEvent* pData);

	// event related
	void ChangeMouseClickState() { (m_mouseClick) ? m_mouseClick = false : m_mouseClick = true; };
	void ChangeMouseHoldingState() { m_mouseHolding ? m_mouseHolding = false : m_mouseHolding = true; };
};

