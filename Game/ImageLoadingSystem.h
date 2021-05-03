#pragma once
#include <unordered_map>

struct SDL_Renderer;
struct SDL_Texture;
struct TextureSpriteSheet;

///////////////////////////////////////////////////////////////////////////////
// robust image loading system that load image from file and store its texture
//////////////////////////////////////////////////////////////////////////////
class ImageLoadingSystem
{
	// Container for load and store image
	// Map[string]Texture
	std::unordered_map<std::string, SDL_Texture*> m_textureMap;
public:
	// constructor and destructor
	ImageLoadingSystem();
	~ImageLoadingSystem();

	// Function to Load Image by file name string, if image exist, return Texture
	//template<class TextureSpriteSheet>
	void Load(SDL_Renderer* pRenderer, const char* pFileName, TextureSpriteSheet* &pSpriteSheet);
	//template<class SDL_Texture>
	void Load(SDL_Renderer* pRenderer, const char* pFileName, SDL_Texture* &pTexture);

	// Delete specific texture
	void DeleteIndicatedTexture(const char* pFileName);

private:
	// Function to unload existing Texture
	void Unload();
};

