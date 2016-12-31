#ifndef RUSTYENGINE_RENDERER_H
#define RUSTYENGINE_RENDERER_H

#pragma once

#include "Component.h"
#include "Sprite.h"
#include "GameObject.h"
#include <SDL.h>

namespace RustyEngine
{
	// Handles rendering of scene object on screen
	// Calculates screen offsets for camera
	class Renderer : public Component
	{
	public:

		GameObject* game_object;	// reference to parent game object
		Sprite* sprite;				// reference to sprite we want to render
		SDL_Renderer* target;		// render target
		GameObject* main_camera;	// main camera
		int w, h;					// render width and height
		bool absolute;

		Renderer(GameObject* g_obj, SDL_Renderer* t, GameObject* cam, Sprite* s, bool a = false);
		~Renderer();

		void update();	// called once per frame at the end of render loop, handles rendering

	};
}

// TODO - move camera and render target references to static
//		- move render width and height to camera

#endif // RUSTYENGINE_RENDERER_H