#include "Sprite.h"
#include <iostream>

// CONSTRUCTORS
Sprite::Sprite()
{
}

Sprite::Sprite(std::string filepath, SDL_Renderer* renderer)
{
	this->load(filepath, renderer);
}



// DESTRUCTOR - Free memory when destroying sprite (unload sprite/texture from RAM and VRAM)
Sprite::~Sprite()
{
	// Free surface from memory if exists
	if (surface != nullptr)
		SDL_FreeSurface(surface);
	// Free texture from memory if exists
	if(texture != nullptr)
		SDL_DestroyTexture(texture);
}


// Load bmp image to surface and create texture from it - returns true if successful
bool Sprite::load(std::string file_name, SDL_Renderer* renderer, int src_x, int src_y)
{
	surface = SDL_LoadBMP(file_name.c_str());

	// todo - if error load primitive
	if (surface == nullptr)
	{
		std::cerr << "Error loading sprite " << file_name << std::endl;
		std::cerr << "Trying to use template instead" << std::endl;
		exit(SDL_SURFACE_ERROR);
		//output;
	}

	// Create texture from surface (textures are stored in VRAM)
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) 
	{
		exit(SDL_TEXTURE_ERROR);
	}

	// Set texture origin of loaded sprite (where sprite is located on texture and how large is it)
	origin.x = src_x;
	origin.y = src_y;
	origin.w = surface->w;
	origin.h = surface->h;

	// Set texture to use alpha blending
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	// Free surface from system memory
	SDL_FreeSurface(surface);
	return true;
}

// Return pointer to texture
SDL_Texture * Sprite::getTexture()
{
	return texture;
}
