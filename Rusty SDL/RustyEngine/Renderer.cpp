#include "Renderer.h"
#include "Game.h"

namespace RustyEngine
{
	// Constructors
	Renderer::Renderer(Sprite * s, bool a)
	{
		sprite = s;
		target = Game::world.main_renderer;

		SDL_RenderGetLogicalSize(target, &w, &h);	// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(target, &w, &h);	// get screen size instead

		absolute = a;
	}

	Renderer::Renderer(SDL_Renderer * t, Sprite * s, bool a)
	{
		sprite = s;
		target = t;
		SDL_RenderGetLogicalSize(target, &w, &h);		// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(target, &w, &h);	// get screen size instead

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
		dest.x = roundf((game_object->transform.position.x * 32) + (w / 2.0f) - (Game::world.active_camera->transform.position.x*32) - (dest.w / 2));
		dest.y = roundf((-game_object->transform.position.y * 32) + (h / 2.0f) + (Game::world.active_camera->transform.position.y*32) - (dest.h / 2));

		// TO-DO move screen-space rendering to GUIRenderer
		if (absolute) // Render at absolute screen position (ignore world, use for GUI, transfer this to GUIRenderer component)
		{
			dest.x = roundf(game_object->transform.position.x);
			dest.y = roundf(game_object->transform.position.y);
		}

		// Render sprite on screen with given rotation (if any)
		SDL_RenderCopyEx(target, sprite->getTexture(), &(sprite->origin), &dest, game_object->transform.getRotation() * RAD_TO_DEG, nullptr, SDL_FLIP_NONE);
	}
}