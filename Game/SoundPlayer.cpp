#include "SoundPlayer.h"
#include <SDL_mixer.h>

SoundPlayer::SoundPlayer()
	: m_pCurrentMusic{ nullptr }
	//, m_pCurrentChunk{nullptr}
	//, m_pPreviousChunk{nullptr}
	, m_pPreviousMusic{nullptr}
	, m_lastPlayingChannel{}
{
	//
}

SoundPlayer::~SoundPlayer()
{
	//m_pCurrentChunk = nullptr;
	m_pCurrentMusic = nullptr;
	//m_pPreviousChunk = nullptr;
	m_pPreviousMusic = nullptr;
}

//void SoundPlayer::SetSoundTrack(Mix_Chunk* pChunk)
//{
//	m_pPreviousChunk = m_pCurrentChunk;
//	m_pCurrentChunk = pChunk;
//}

void SoundPlayer::SetSoundTrack(Mix_Music* pMusic)
{
	m_pPreviousMusic = m_pCurrentMusic;
	m_pCurrentMusic = pMusic;
}

void SoundPlayer::PlayChunk(Mix_Chunk* pChunk, bool fadeIn, int playingLoop, int fadingLength, int channel)
{
	if (fadeIn)
		Mix_FadeInChannel(channel, pChunk, playingLoop, fadingLength);
	else
		Mix_PlayChannel(channel, pChunk, playingLoop);
}

void SoundPlayer::PlayMusic(bool fadeIn, int playingLoop, int fadingLength)
{
	if (fadeIn)
		Mix_FadeInMusic(m_pCurrentMusic, playingLoop, fadingLength);
	else
		Mix_PlayMusic(m_pCurrentMusic, playingLoop);
}
