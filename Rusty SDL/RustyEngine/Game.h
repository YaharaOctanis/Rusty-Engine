#ifndef RUSTYENGINE_GAME_H
#define RUSTYENGINE_GAME_H

#pragma once

#include <SDL.h>
#include "World.h"

namespace RustyEngine
{
	// Main engine class
	// Handles initialization, update and physics loops, render and cleanups.
	// Call init() in this class before doing anything else in the engine!
	// Init function will change function called atexit(). 
	// If you want to write custom exit function, move quit to public and call it from your custom exit function.
	class Game
	{
	private:
		static Uint32 max_lthreads;		// Number of avalible logical threads (at least 1)

		static void init_SDL();			// Initialize SDL (also create game renderer and system window)
		static bool verCheck();			// Check if linked SDL library matches compiled version (TRUE if major versions match | FALSE if minor versions mismatch | ERROR/EXIT if major versions mismatch)
		static void queryCpuInfo();		// Check system capabilities (special instructions support, number of logical threads, cpu speed, ...)
		static void quit();				// Clean up all initialized subsystems and SDL
	public:
		static World world;		// Game world
		static void init();		// Initialize game engine
		static void render();	// Render game world (SDL render command)

	private:
		Game();
		~Game();
	};

}

#endif // RUSTYENGINE_GAME_H