#include "SoundLoadingSystem.h"
#include <SDL_mixer.h>
#include <iostream>

SoundLoadingSystem::SoundLoadingSystem()
{
	//
}

SoundLoadingSystem::~SoundLoadingSystem()
{
	UnloadAll();
	m_chunkMap.clear();
	m_musicMap.clear();
}

//-----------------------------------------------------------------
//	overload function that load chunk from file
//	@Param:
//	- file path to load the sound
//	- a chunk file pointer that points to the loaded chunk
//-----------------------------------------------------------------
void SoundLoadingSystem::Load(const char* filePath, Mix_Chunk*& chunkFile)
{
	// if the loading chunk is already exist in our system, return the chunk
	if (m_chunkMap.find(filePath) != m_chunkMap.end())
	{
		chunkFile = m_chunkMap[filePath];
		return;
	}

	// else, load the chunk
	chunkFile = Mix_LoadWAV(filePath);

	// if not successful, show error and go back
	if (!chunkFile)
	{
		std::cout<< "failed to load chunk. Error: " << Mix_GetError() << std::endl;
		return;
	}

	// if successful, add the chunk to chunk map
	m_chunkMap.emplace(filePath, chunkFile);
}

//-----------------------------------------------------------------
//	overload function that load music from file
//	@Param:
//	- file path to load the music
//	- a music file pointer that points to the loaded music
//-----------------------------------------------------------------
void SoundLoadingSystem::Load(const char* filePath, Mix_Music*& musicFile)
{
	// if the loading music is already exist in our system, return the music
	if (m_musicMap.find(filePath) != m_musicMap.end())
	{
		musicFile = m_musicMap[filePath];
		return;
	}

	// else, load the music
	musicFile = Mix_LoadMUS(filePath);

	// if not successful, show error and go back
	if (!musicFile)
	{
		std::cout << "failed to load music. Error: " << Mix_GetError() << std::endl;
		return;
	}

	// if successful, add the music to music map
	m_musicMap.emplace(filePath, musicFile);
}

void SoundLoadingSystem::UnloadAll()
{
	// unload chunk
	for (auto it = m_chunkMap.begin(); it != m_chunkMap.end(); ++it)
	{
		Mix_FreeChunk(it->second);
		it->second = nullptr;
	}

	// unload music
	for (auto it = m_musicMap.begin(); it != m_musicMap.end(); ++it)
	{
		Mix_FreeMusic(it->second);
		it->second = nullptr;
	}
}
