#ifndef RUSTYENGINE_WORLD_H
#define RUSTYENGINE_WORLD_H

#pragma once

#include "Level.h"
#include "Transform.h"
#include <SDL.h>
#include <string>

namespace RustyEngine
{
	// Representation of game world
	// Can hold and process multiple levels
	// Holds pointers to game window and renderer, as well as currently active_camera.
	class World
	{
	public:
		// Public properties
		string name;					// World can be given a name, this will also be a window title
		vector<Level*> levels;			// Reference to every level in the world (game)
		GameObject *active_camera;		// Current main camera
		GameObject *audio_listener;		// Current audio listener (used for positional audio calculation)
		SDL_Renderer *main_renderer;	// SDL renderer reference
		SDL_Window *main_window;		// Game SDL window reference

	public:
		// Constructors
		World();
		World(string name);

		// Destructor
		~World();

		// Initialize
		void init();	// Create window and SDL renderer (called from game.h) TODO, call this via friendship class

		// Update
		void update();	// Update every level
	};
}

#endif // RUSTYENGINE_WORLD_H