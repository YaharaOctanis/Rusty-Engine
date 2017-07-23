#ifndef RUSTYENGINE_SPRITE_H
#define RUSTYENGINE_SPRITE_H

#pragma once

#include <SDL.h>
#include <string>
#include "Vec2.h"
#include "Error.h"

namespace RustyEngine
{
	// Sprite class holds image information stored in texture (VRAM) and handles it's loading and unloading

	class Sprite
	{
	private:
		SDL_Texture* texture;		// Texture reference
	public:
		std::string file;			// Filepath of loaded sprite
		SDL_Rect origin;			// Origin of loaded sprite (where is sprite located on the image, what size is it?)

		// TODO finish texture atlas implementation

		// Sprite atlas properties
		Vec2 grid_size;				// How many collumns and rows
		Vec2 cell_size;				// Size of the each cell in a grid
		Vec2 grid_offset;			// How many pixels away from the border does the gird start
		Vec2 cell_padding;			// Amount of empty space after each cell

	public:
		// Constructors
		Sprite();
		Sprite(std::string filepath);
		Sprite(std::string filepath, SDL_Renderer* renderer);

		// Destructor
		~Sprite();

		// Load bmp image to surface and create texture from it - returns true if successful
		bool load(std::string file_name, SDL_Renderer* renderer, int src_x = 0, int src_y = 0);

		// todo - methods for working with sprite, mostly rendering and cleanup
		SDL_Texture* getTexture();			// Return pointer to texture
		Vec2 getCell(Vec2 grid_position);	// Get pixel position of the cell on the texture
		Vec2 getCell(int grid_index);		// Get pixel position of the cell on the texture
	};
}

#endif // RUSTYENGINE_SPRITE_H