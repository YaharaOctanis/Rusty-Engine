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
	private:
		

	public:
		SDL_Rect origin;			// sprite's origin on texture, and size

		//GameObject* game_object;	// reference to parent game object (now inhereted :O)
		Sprite* sprite;				// reference to sprite we want to render
		SDL_Renderer* target;		// render target
		//GameObject* main_camera;	// main camera
		int w, h;					// render width and height
		bool absolute;
		Vec2 grid_pos;				// Grid position of the sprite to render
		bool flip_x, flip_y;		// flip sprite on given axis if true

		Renderer(Sprite* s, bool a = false);
		Renderer(SDL_Renderer* t, Sprite* s, bool a = false);
		~Renderer();


		void update();	// called once per frame at the end of render loop, handles rendering

	};
}

// TODO - create camera class
//		- move render width and height to camera
//		- cleanup comments in this file

#endif // RUSTYENGINE_RENDERER_H