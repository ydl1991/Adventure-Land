#include "ImageLoadingSystem.h"
#include "TextureSpriteSheet.h"
#include <SDL_image.h>
#include <SDL.h>

ImageLoadingSystem::ImageLoadingSystem()
{
}

ImageLoadingSystem::~ImageLoadingSystem()
{
	// destroy texture and clear container
	Unload();
}

// call to load image to the container map
void ImageLoadingSystem::Load(SDL_Renderer* pRenderer, const char* pFileName, TextureSpriteSheet* &pSpriteSheet)
{
	// load image and store and store info
	SDL_Surface* pTempSurface = IMG_Load(pFileName);
	int width = pTempSurface->w;
	int height = pTempSurface->h;
	// if image already exist, return image
	if (m_textureMap.find(pFileName) != m_textureMap.end())
	{
		pSpriteSheet = new TextureSpriteSheet{ m_textureMap[pFileName], width, height };
		SDL_FreeSurface(pTempSurface);
		return;
	}
	// create texture
	if (pTempSurface)
	{
		SDL_Texture* pTemp = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);
		// if load image successfully, emplace back to container
		m_textureMap.emplace(pFileName, pTemp);
		pSpriteSheet = new TextureSpriteSheet{ pTemp, width, height };
		SDL_FreeSurface(pTempSurface);
		pTemp = nullptr;
	}
}

// call to load image to the container map
void ImageLoadingSystem::Load(SDL_Renderer* pRenderer, const char* pFileName, SDL_Texture* &pTexture)
{
	// if image already exist, return image
	if (m_textureMap.find(pFileName) != m_textureMap.end())
	{
		pTexture = m_textureMap[pFileName];
		return;
	}
	// load image and store in the container
	SDL_Surface* pTempSurface = IMG_Load(pFileName);
	if (pTempSurface)
	{
		SDL_Texture* pTemp = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);
		// if load image successfully, emplace back to container
		m_textureMap.emplace(pFileName, pTemp);
		pTexture = pTemp;
		SDL_FreeSurface(pTempSurface);
		pTemp = nullptr;
	}
}

// destroy specific texture and erase from the map
void ImageLoadingSystem::DeleteIndicatedTexture(const char* pFileName)
{
	// if image exist, remove
	if (auto it = m_textureMap.find(pFileName); it != m_textureMap.end())
	{
		SDL_DestroyTexture(it->second);
		m_textureMap.erase(it);
	}
}

// call to destroy all texture
void ImageLoadingSystem::Unload()
{
	for (auto it = m_textureMap.begin(); it != m_textureMap.end(); ++it)
	{
		SDL_DestroyTexture(it->second);
		it->second = nullptr;
	}

	// clear map
	m_textureMap.clear();
}
