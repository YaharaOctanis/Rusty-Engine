#ifndef RUSTYENGINE_WORLD_H
#define RUSTYENGINE_WORLD_H

#pragma once

#include "Level.h"
#include "Transform.h"
#include <SDL.h>
#include <string>

namespace RustyEngine
{
	class World
	{
	public:
		string name;
		vector<Level*> levels;
		GameObject *active_camera;
		SDL_Renderer *main_renderer;
		SDL_Window *main_window;

	public:
		World();
		World(string name);
		~World();

		void init();	// Create window and SDL renderer
		void update();
	};
}

#endif // RUSTYENGINE_WORLD_H