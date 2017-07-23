#include "Sprite.h"
#include <iostream>
#include "Game.h"

using namespace RustyEngine;

namespace RustyEngine
{
	// CONSTRUCTORS
	Sprite::Sprite()
	{
		grid_size.set(1, 1);
	}

	Sprite::Sprite(std::string filepath)
	{
		grid_size.set(1, 1);
		this->load(filepath, Game::world.main_renderer);
	}

	Sprite::Sprite(std::string filepath, SDL_Renderer* renderer)
	{
		grid_size.set(1, 1);
		this->load(filepath, renderer);
	}

	
	// DESTRUCTOR - Free memory when destroying sprite (unload sprite/texture from RAM and VRAM)
	Sprite::~Sprite()
	{
		// Free texture from memory if exists
		if (texture != nullptr)
			SDL_DestroyTexture(texture);
	}


	// Load bmp image to surface and create texture from it - returns true if successful
	bool Sprite::load(std::string file_name, SDL_Renderer* renderer, int src_x, int src_y)
	{
		SDL_Surface* surface;		// Surface reference required at loading (TODO move to load function)
		surface = SDL_LoadBMP(file_name.c_str());

		// todo - if error load primitive
		if (surface == nullptr)
		{
			std::cerr << "SPRITE | Error loading sprite " << file_name << std::endl;
			std::cerr << "SPRITE | Trying to use template instead" << std::endl;
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
		cell_size.set(origin.w, origin.h);

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

	// Get pixel position of the cell on the texture
	Vec2 Sprite::getCell(Vec2 grid_position)
	{
		Vec2 pos;
		pos.x = grid_offset.x + (grid_position.x * (cell_size.x + cell_padding.x));
		pos.y = grid_offset.y + (grid_position.y * (cell_size.y + cell_padding.y));
		return pos;
	}

	Vec2 Sprite::getCell(int grid_index)
	{
		Vec2 pos;
		int grid_x = grid_index % static_cast<int>(grid_size.x);
		int grid_y = grid_index / grid_size.x;
		pos.x = grid_offset.x + (grid_x * (cell_size.x + cell_padding.x));
		pos.y = grid_offset.y + (grid_y * (cell_size.y + cell_padding.y));
		return pos;
	}
}