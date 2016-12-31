#include "Renderer.h"

using namespace RustyEngine;

// Constructor
Renderer::Renderer(GameObject * g_obj, SDL_Renderer * t, GameObject * cam, Sprite * s, bool a)
{
	sprite = s;
	main_camera = cam;
	target = t;
	SDL_RenderGetLogicalSize(target, &w, &h);		// get render target size

	if(w == 0 || h == 0)		// if no render target size
		SDL_GetRendererOutputSize(target, &w, &h);	// get screen size instead

	game_object = g_obj;
	absolute = a;
}

// Destructor
Renderer::~Renderer()
{
}

// Called once per frame at the end of render loop, handles rendering
void Renderer::update()
{
	SDL_Rect dest;	// Where on screen to render & at what size
	dest.w = sprite->origin.w * game_object->transform.getScale().x;	// Calculate target width of sprite
	dest.h = sprite->origin.h * game_object->transform.getScale().y;	// Calculate target height of sprite

	// Calculate position of sprite on screen (can be off-screen)
	dest.x = roundf(game_object->transform.position.x + (w / 2.0f) - main_camera->transform.position.x - (dest.w/2));
	dest.y = roundf(-game_object->transform.position.y + (h / 2.0f) + main_camera->transform.position.y - (dest.h/2));

	if (absolute)
	{
		dest.x = roundf(game_object->transform.position.x);
		dest.y = roundf(game_object->transform.position.y);
	}
	
	// Render sprite on screen with given rotation (if any)
	SDL_RenderCopyEx(target, sprite->getTexture(), &(sprite->origin), &dest, game_object->transform.getRotation(), nullptr, SDL_FLIP_NONE);
}
