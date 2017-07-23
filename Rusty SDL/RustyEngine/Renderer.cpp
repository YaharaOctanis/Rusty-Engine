#include "Renderer.h"
#include "Game.h"
#include "RustyConstants.h"

namespace RustyEngine
{
	// Constructors
	Renderer::Renderer(Sprite * s, bool a)
	{
		flip_x = false;
		flip_y = false;
		sprite = s;
		target = Game::world.main_renderer;
		origin = s->origin;

		SDL_RenderGetLogicalSize(target, &w, &h);	// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(target, &w, &h);	// get screen size instead

		absolute = a;
	}

	Renderer::Renderer(SDL_Renderer * t, Sprite * s, bool a)
	{
		flip_x = false;
		flip_y = false;
		sprite = s;
		target = t;
		origin = s->origin;

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
		origin.w = sprite->cell_size.x;
		origin.h = sprite->cell_size.y;
		Vec2 pix_pos = sprite->getCell(grid_pos);
		origin.x = pix_pos.x;
		origin.y = pix_pos.y;

		SDL_Rect dest;	// Where on screen to render & at what size
		dest.w = origin.w * game_object->transform.getScale().x * RENDER_SCALE;	// Calculate target width of sprite
		dest.h = origin.h * game_object->transform.getScale().y * RENDER_SCALE;	// Calculate target height of sprite

		if (RENDER_PIXEL_PERFECT)
		{
			Vec2 temp_vec(1, 0), temp2_vec(0, 0);
			Vec2 go_pos;
			Vec2 cam_pos;
			float pixel_size = (Game::world.screenToWorldSpace(temp_vec) - Game::world.screenToWorldSpace(temp2_vec)).x;
			//pixel_size *= 128;
			//pixel_size *= RENDER_SCALE;

			if (pixel_size != 0)
			{
				go_pos.x = game_object->transform.position.x - (fmodf(game_object->transform.position.x / pixel_size, 1) * pixel_size);
				go_pos.y = game_object->transform.position.y - (fmodf(game_object->transform.position.y / pixel_size, 1) * pixel_size);
				cam_pos.x = Game::world.active_camera->transform.position.x - (fmodf(Game::world.active_camera->transform.position.x / pixel_size, 1) * pixel_size);
				cam_pos.y = Game::world.active_camera->transform.position.y - (fmodf(Game::world.active_camera->transform.position.y / pixel_size, 1) * pixel_size);
			}

			dest.x = ceilf((go_pos.x * 32 * RENDER_SCALE) + (w / 2.0f) - (cam_pos.x * 32 * RENDER_SCALE) - (dest.w / 2));
			dest.y = ceilf((-go_pos.y * 32 * RENDER_SCALE) + (h / 2.0f) + (cam_pos.y * 32 * RENDER_SCALE) - (dest.h / 2));
		}
		else
		{
			// Calculate position of sprite on screen (can be off-screen)
			dest.x = roundf((game_object->transform.position.x * 32 * RENDER_SCALE) + (w / 2.0f) - (Game::world.active_camera->transform.position.x * 32 * RENDER_SCALE) - (dest.w / 2));
			dest.y = roundf((-game_object->transform.position.y * 32 * RENDER_SCALE) + (h / 2.0f) + (Game::world.active_camera->transform.position.y * 32 * RENDER_SCALE) - (dest.h / 2));
		}

		// TO-DO move screen-space rendering to GUIRenderer
		if (absolute) // Render at absolute screen position (ignore world, use for GUI, transfer this to GUIRenderer component)
		{
			dest.w = roundf(origin.w * game_object->transform.getScale().x);
			dest.h = roundf(origin.h * game_object->transform.getScale().y);
			dest.x = roundf(game_object->transform.position.x);
			dest.y = roundf(game_object->transform.position.y);
		}

		// Render sprite on screen with given rotation (if any)
		if(!flip_x && !flip_y)
			SDL_RenderCopyEx(target, sprite->getTexture(), &(origin), &dest, game_object->transform.getRotation() * RAD_TO_DEG, nullptr, SDL_FLIP_NONE);
		else if(flip_x && !flip_y)
			SDL_RenderCopyEx(target, sprite->getTexture(), &(origin), &dest, game_object->transform.getRotation() * RAD_TO_DEG, nullptr, SDL_FLIP_HORIZONTAL);
		else if (!flip_x && flip_y)
			SDL_RenderCopyEx(target, sprite->getTexture(), &(origin), &dest, game_object->transform.getRotation() * RAD_TO_DEG, nullptr, SDL_FLIP_VERTICAL);
		else if (flip_x && flip_y)
			SDL_RenderCopyEx(target, sprite->getTexture(), &(origin), &dest, (game_object->transform.getRotation() * RAD_TO_DEG) + 90, nullptr, SDL_FLIP_NONE);
	}
}