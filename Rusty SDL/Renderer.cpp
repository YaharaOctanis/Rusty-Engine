#include "Renderer.h"

Renderer::Renderer(GameObject * g_obj, SDL_Renderer * t, GameObject * cam, Sprite * s)
{
	sprite = s;
	main_camera = cam;
	target = t;
	SDL_RenderGetLogicalSize(target, &w, &h);

	if(w == 0 || h == 0)
		SDL_GetRendererOutputSize(target, &w, &h);

	game_object = g_obj;
}

Renderer::~Renderer()
{
}

void Renderer::update()
{
	SDL_Rect dest;
	dest.w = sprite->origin.w * game_object->transform.getScale().x;
	dest.h = sprite->origin.h * game_object->transform.getScale().y;

	dest.x = roundf(game_object->transform.position.x + (w / 2.0f) - main_camera->transform.position.x - (dest.w/2));
	dest.y = roundf(game_object->transform.position.y + (h / 2.0f) + main_camera->transform.position.y - (dest.h/2));
	
	SDL_RenderCopyEx(target, sprite->getTexture(), &(sprite->origin), &dest, game_object->transform.getRotation(), nullptr, SDL_FLIP_NONE);
	//SDL_RenderCopy(target, sprite->getTexture(), &(sprite->origin), &dest);
	//SDL_RenderCopy(renderer, robo.texture, src_rect, dest_rect);
}
