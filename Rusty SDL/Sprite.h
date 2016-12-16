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
	SDL_Surface* surface;
	SDL_Texture* texture;
public:
	std::string file;
	SDL_Rect origin;

public:
	Sprite();
	Sprite(std::string filepath, SDL_Renderer* renderer);
	~Sprite();

	// Load bmp image to surface and create texture from it - returns true if successful
	bool Load(std::string file_name, SDL_Renderer* renderer, int x, int y);

	// todo - methods for working with sprite, mostly rendering and cleanup
};

#endif // SPRITE_H