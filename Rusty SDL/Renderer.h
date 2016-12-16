#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#include "Component.h"
#include "Sprite.h"
#include "GameObject.h"
#include <SDL.h>

class Renderer : public Component
{
public:

	GameObject* game_object;
	Sprite* sprite;
	SDL_Renderer* target;
	GameObject* main_camera;
	int w, h;

	Renderer(GameObject* g_obj, SDL_Renderer* t, GameObject* cam, Sprite* s);
	~Renderer();

	void update();

};

#endif // RENDERER_H