#include "ReadWriteSystem.h"
#include "SimpleSDLGame.h"
#include "StateMachine.h"
#include "GameRunningState.h"
#include "WorldBackground.h"
#include "BaseState.h"
#include "EnemyBoss.h"
#include "SpawnerSystem.h"
#include "Player.h"
#include <direct.h>
#include <fstream>
#include <iostream>
#include <string>
#include <optional>
#include <stdio.h>

ReadWriteSystem::ReadWriteSystem(SimpleSDLGame* pGameWorld)
	: m_isLoaded{ false }
	, m_data{}
	, m_pGameWorld{ pGameWorld }
	, m_isSlotOneDirty{ true }
	, m_isSlotTwoDirty{ true }
	, m_isSlotThreeDirty{ true }
	, m_numOfFilesSaved{0}
{
	UpdateSavedData();
}

ReadWriteSystem::~ReadWriteSystem()
{
}

bool ReadWriteSystem::Save(int slot)
{
	if (slot <= 0 && slot > m_kTotalFilesCanSave)
	{
		std::cout << "Invalid Game Slot!" << std::endl;
		return false;
	}

	// Collect and Save info
	CollectAndFormSaveData(m_data[slot - 1]);

	// Make directory (make folder)
	int error = _mkdir(s_kDirectory);

	// combine the directory and file name
	std::string path = s_kDirectory + std::string(m_kFileName[slot - 1]);

	// Remove old file
	remove(path.c_str());

	// rewrite file
	std::ofstream file;
	file.open(path, std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		std::cout << "Failed to open file " << path << std::endl;
		return false;
	}

	// Write the whole data struct in one swoop!
	file.write((char*)&m_data[slot - 1], sizeof(m_data[slot - 1]));

	// close file when down
	file.close();

	// everytime save a file, +1 to make sure it's not 0;
	++m_numOfFilesSaved;

	// check which slot is dirty
	slot == 1 ? m_isSlotOneDirty = true : slot == 2 ? m_isSlotTwoDirty = true : m_isSlotThreeDirty = true;
	return true;
}

bool ReadWriteSystem::Load(int slot)
{
	// if invalid slot, return
	if (slot <= 0 && slot > m_kTotalFilesCanSave)
	{
		std::cout << "Invalid Game Slot!" << std::endl;
		return false;
	}

	// if file finished loading and slot is a occupided slot, process
	if (m_isLoaded && m_data[slot - 1].m_isOccupied)
	{
		GameRunningState* pRunningState = dynamic_cast<GameRunningState*>(m_pGameWorld->GetStateMachine()->GetCurrentState());
		EnemyBoss* pBoss = pRunningState->GetBoss();

		// clear out before load
		if(m_pGameWorld->GetStateMachine()->GetCurrentState()->GetStateType() != BaseState::StateType::kGameMenuState)
			pRunningState->LoadGamePreparation();

		// retrieve data and apply to game
		Player* pPlayer = pRunningState->GetPlayer();
		pRunningState->GetBackground()->SetCameraLocation(m_data[slot - 1].m_mapLocation);
		pRunningState->m_playerType = m_data[slot - 1].m_playerType;
		pPlayer->SetCoordinateVector(m_data[slot - 1].m_playerLocation);
		pPlayer->SetHealth(m_data[slot - 1].m_playerHealth);
		pPlayer->SetWeaponLevel(m_data[slot - 1].m_weaponLevel);
		pRunningState->GetSpawnerSystem()->CheckWeaponComboInit();

		if (pBoss && m_data[slot - 1].m_isBossExist)
		{
			pBoss->Validate();
			pBoss->SetCoordinateVector(m_data[slot - 1].m_bossLocation);
			pBoss->SetHealth(m_data[slot - 1].m_bossHealth);
			pBoss->SetMana(m_data[slot - 1].m_bossMana);
		}
		else if (pBoss && !m_data[slot - 1].m_isBossExist)
		{
			pBoss->Invalidate();
		}
		
		// File finished loading
		m_isLoaded = false;
		return true;
	}
	else
	{
		std::cout << "Error! File might no exist or might not finished loading!" << std::endl;
		return false;
	}
}

//--------------------------------------
// Everytime when enter Save and Load menu,
// make sure to update game file
//--------------------------------------
void ReadWriteSystem::UpdateSavedData()
{
	std::ifstream file;

	// slot one dirty reload slot one
	if (m_isSlotOneDirty)
	{
		std::string path = s_kDirectory + std::string(m_kFileName[0]);
		file.open(path, std::ios::in);
		if (!file.is_open())
			std::cout << "Failed to load file " << path << "! Check if file exists!" << std::endl;
		else
			file.read((char*)&m_data[0], sizeof(m_data[0]));

		file.close();

		// up to date
		m_isSlotOneDirty = false;
	}

	// slot two dirty reload slot two
	if (m_isSlotTwoDirty)
	{
		std::string path = s_kDirectory + std::string(m_kFileName[1]);
		file.open(path, std::ios::in);
		if (!file.is_open())
			std::cout << "Failed to load file " << path << "! Check if file exists!" << std::endl;
		else
			file.read((char*)&m_data[1], sizeof(m_data[1]));

		file.close();

		// up to date
		m_isSlotTwoDirty = false;
	}

	// slot three dirty reload slot three
	if (m_isSlotThreeDirty)
	{
		std::string path = s_kDirectory + std::string(m_kFileName[2]);
		file.open(path, std::ios::in);
		if (!file.is_open())
			std::cout << "Failed to load file " << path << "! Check if file exists!" << std::endl;
		else
			file.read((char*)&m_data[2], sizeof(m_data[2]));

		file.close();

		// up to date
		m_isSlotThreeDirty = false;
	}

	// check files number 
	m_numOfFilesSaved = 0;
	for (int i = 0; i < m_kTotalFilesCanSave; ++i)
	{
		if (m_data[i].m_isOccupied)
			++m_numOfFilesSaved;
	}

	// File successfully loaded
	m_isLoaded = true;
}

//--------------------------------------
// Collect data from the game state and 
// form a SaveData for game save
//--------------------------------------
bool ReadWriteSystem::CollectAndFormSaveData(ReadWriteSystem::SaveData& data)
{
	// only collect game data while in game running state
	if (m_pGameWorld->GetStateMachine()->GetCurrentState()->GetStateType() != BaseState::StateType::kGameRunningState)
	{
		std::cout << "Error Operation!" << std::endl;
		return false;
	}

	// Get the game running state
	GameRunningState* pRunningState = dynamic_cast<GameRunningState*>(m_pGameWorld->GetStateMachine()->GetCurrentState());
	SDL_FRect mapRect = pRunningState->GetBackground()->GetCameraDimention();
	EnemyBoss* pBoss = pRunningState->GetBoss();

	// Update game info
	Player* pPlayer = pRunningState->GetPlayer();
	data.m_isOccupied = true;
	data.m_mapLocation = { mapRect.x, mapRect.y};
	data.m_playerType = pRunningState->m_playerType;
	data.m_playerLocation = pPlayer->GetCoordinateVector();
	data.m_playerHealth = pPlayer->GetHealth();
	data.m_weaponLevel = pPlayer->GetWeaponLevel();

	if (pBoss && pBoss->IsValid())
	{
		data.m_isBossExist = true;
		data.m_bossLocation = pBoss->GetCoordinateVector();
		data.m_bossHealth = pBoss->GetHealth();
		data.m_bossMana = pBoss->GetMana();
	}

	// verify process
	return true;
}
