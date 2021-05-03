#pragma once
#include <unordered_map>
#include <SDL_mixer.h>

///////////////////////////////////////////////////////////////////////////////
// robust sound loading system that load image from file and store its texture
//////////////////////////////////////////////////////////////////////////////
class SoundLoadingSystem
{
	// Container for load and store sounds
	std::unordered_map<std::string, Mix_Chunk*> m_chunkMap;			// Map[string]chunk
	std::unordered_map<std::string, Mix_Music*> m_musicMap;			// Map[string]music
public:
	SoundLoadingSystem();
	~SoundLoadingSystem();

	// overload functions that load the sounds
	void Load(const char* filePath, Mix_Chunk* &chunkFile);			// load chunk
	void Load(const char* filePath, Mix_Music* &musicFile);			// load music

private:
	// clear out memory
	void UnloadAll();												// clear all chunks and musics
};

