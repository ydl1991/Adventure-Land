#pragma once
#include "Vector2D.h"
#include <string>

class SimpleSDLGame;

class ReadWriteSystem
{
public:
	struct SaveData
	{
		// file Occupacy
		bool m_isOccupied;
		// record map info
		Vector2D m_mapLocation;
		// record player info
		int m_playerType;
		Vector2D m_playerLocation;
		float m_playerHealth;
		int m_weaponLevel;
		// record boss info
		bool m_isBossExist;
		Vector2D m_bossLocation;
		float m_bossHealth;
		float m_bossMana;

		// Constructor
		SaveData() : m_isOccupied{ false }, m_mapLocation{}, m_playerType{}, m_playerLocation{}
					, m_playerHealth{}, m_weaponLevel{}, m_isBossExist{ false }
					, m_bossLocation{}, m_bossHealth{}, m_bossMana{}{}
		// Destructor
		~SaveData() {}
	};

	// flag value determine the total number can be saved
	static constexpr int m_kTotalFilesCanSave = 3;

private:

	SaveData m_data[m_kTotalFilesCanSave];
	bool m_isLoaded;

	const char* m_kFileName[m_kTotalFilesCanSave] = { "FileSave1.save", "FileSave2.save", "FileSave3.save" };
	static constexpr char s_kDirectory[] = "saves\\";

	SimpleSDLGame* m_pGameWorld;

	// flags to determine updated game files
	bool m_isSlotOneDirty;
	bool m_isSlotTwoDirty;
	bool m_isSlotThreeDirty;

	int m_numOfFilesSaved;

public:

	ReadWriteSystem(SimpleSDLGame* pGameWorld);
	~ReadWriteSystem();

	// saves the files. Return true if successful
	// when save, you get your player's current location and save your player's current location
	bool Save(int slot);
	// Loads the file. Returns true if successful
	// load at the place when init the world( or game running state) before creating all object
	bool Load(int slot);

	// Retrieve different files
	SaveData GetFileOne() const { return m_data[0]; };
	SaveData GetFileTwo() const { return m_data[1]; };
	SaveData GetFileThree() const { return m_data[2]; };

	bool IsLoaded() const { return m_isLoaded; }

	// Update Saved Info
	void UpdateSavedData();

	// Get File Saving Status
	int GetNumOfFileSaved() const { return m_numOfFilesSaved; }

private:
	// collect saving data from the world
	bool CollectAndFormSaveData(SaveData& data);
};

