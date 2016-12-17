#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include <SDL.h>
#include <string>
#include "Vec2.h"
#include "Error.h"

// Sprite class holds image information stored in texture (VRAM) and handles it's loading and unloading
class Sprite
{
private:
	SDL_Surface* surface;		// Surface reference required at loading (TODO move to load function)
	SDL_Texture* texture;		// Texture reference
public:
	std::string file;			// Filepath of loaded sprite
	SDL_Rect origin;			// Origin of loaded sprite (where is sprite located on the image, what size is it?)
								// Used for texture atlas
								// TODO implement texture atlas

public:
	// Constructors
	Sprite();
	Sprite(std::string filepath, SDL_Renderer* renderer);

	// Destructor
	~Sprite();

	// Load bmp image to surface and create texture from it - returns true if successful
	bool load(std::string file_name, SDL_Renderer* renderer, int src_x=0, int src_y=0);

	// todo - methods for working with sprite, mostly rendering and cleanup
	SDL_Texture* getTexture();
};

#endif // SPRITE_H