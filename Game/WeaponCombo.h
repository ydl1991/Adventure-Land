#pragma once
#include "BlueMagicBullet.h"
#include "MagicWave.h"
#include "Projectile.h"
#include "SmallOpticalWave.h"
#include <vector>

//--------------------------------------------------------------
//	Different Weapon Combo According to Different Weapon Level
//--------------------------------------------------------------
class WeaponCombo 
{
protected:
	std::vector<Projectile*> m_bullets;

public:
	WeaponCombo() {};
	virtual ~WeaponCombo() 
	{
		for (auto it : m_bullets)
		{
			delete it;
			it = nullptr;
		}
	};

	void Update(float deltaTime)
	{
		for (auto it : m_bullets)
		{
			it->Tick(deltaTime);
		}
	};

	void Render(SDL_Renderer* pRenderer)
	{
		for (auto it : m_bullets)
		{
			it->Render(pRenderer);
		}
	};

	void Reset(Vector2D playerCoordinate)
	{
		for (auto it : m_bullets)
		{
			it->SetCoordinateVector(playerCoordinate);
			it->GetCollisionSystem()->SetPosition(playerCoordinate);
			it->Validate();
		}
	};

	void LevelThreeReset(Vector2D firePointOne, Vector2D firePointTwo)
	{
		m_bullets[0]->SetCoordinateVector(firePointOne);
		m_bullets[0]->GetCollisionSystem()->SetPosition(firePointOne);
		m_bullets[0]->Validate();
		
		m_bullets[1]->SetCoordinateVector(firePointTwo);
		m_bullets[1]->GetCollisionSystem()->SetPosition(firePointTwo);
		m_bullets[1]->Validate();

		m_bullets[2]->SetCoordinateVector(firePointTwo);
		m_bullets[2]->GetCollisionSystem()->SetPosition(firePointTwo);
		m_bullets[2]->Validate();
	};
};

class LevelOneCombo : public WeaponCombo
{
public:
	LevelOneCombo(Projectile* pOne) { m_bullets.emplace_back(pOne); }
	~LevelOneCombo() {}
};

class LevelTwoCombo : public WeaponCombo
{
public:
	LevelTwoCombo(MagicWave* pOne) { m_bullets.emplace_back(pOne); }
	~LevelTwoCombo() {}
};

class LevelThreeCombo : public WeaponCombo
{
public:
	LevelThreeCombo(MagicWave* pOne, SmallOpticalWave* pTwo, SmallOpticalWave* pThree)
	{
		m_bullets.emplace_back(pOne);
		m_bullets.emplace_back(pTwo);
		m_bullets.emplace_back(pThree);
	}

	~LevelThreeCombo() {}
};