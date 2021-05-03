#pragma once
#include <SDL_mixer.h>
#include <unordered_map>

class SoundPlayer
{
	// sound source variables
	//Mix_Chunk* m_pCurrentChunk;
	Mix_Music* m_pCurrentMusic;
	//Mix_Chunk* m_pPreviousChunk;
	Mix_Music* m_pPreviousMusic;

	int m_lastPlayingChannel;

public:
	SoundPlayer();
	~SoundPlayer();

	// setters
	//void SetSoundTrack(Mix_Chunk* pChunk);
	void SetSoundTrack(Mix_Music* pMusic);

	// getters
	//Mix_Chunk* GetCurrentPlayingChunk() const { return m_pCurrentChunk; }
	Mix_Music* GetCurrentPlayingMusic() const { return m_pCurrentMusic; }
	//Mix_Chunk* GetPreviousPlayingChunk() const { return m_pPreviousChunk; }
	Mix_Music* GetPreviousPlayingMusic() const { return m_pPreviousMusic; }

	// Playing sound function
	void PlayChunk(Mix_Chunk* pChunk, bool fadeIn, int playingLoop, int fadingLength = 1, int channel = -1);
	void PlayMusic(bool fadeIn, int playingLoop, int fadingLength = 0);
};

