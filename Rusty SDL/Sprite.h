#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include <SDL.h>
#include <string>
#include "Vec2.h"
#include "Error.h"

class Sprite
{
private:
	std::string file;
	Vec2 origin;
	SDL_Surface* surface;
	SDL_Texture* texture;

public:
	Sprite();
	Sprite(std::string filepath);
	~Sprite();

	// Load bmp image to surface and create texture from it - returns true if successful
	bool Load(std::string file_name);
	// todo - methods for working with sprite, mostly rendering and cleanup
};

#endif // SPRITE_H