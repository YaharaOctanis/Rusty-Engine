#include "Sprite.h"
#include <iostream>


Sprite::Sprite()
{
}

Sprite::Sprite(std::string filepath)
{

}


Sprite::~Sprite()
{
}

// Load bmp image to surface and create texture from it - returns true if successful
bool Sprite::Load(std::string file_name)
{
	SDL_Surface* output = nullptr;

	output = SDL_LoadBMP(file_name.c_str());

	// todo - if error load primitive
	if (output == nullptr)
	{
		std::cerr << "Error loading sprite " << file_name << std::endl;
		std::cerr << "Using template instead" << std::endl;
		exit(SDL_TEXTURE_ERROR);
		//output;
	}

	// todo - create texture from surface

	return output;
}
