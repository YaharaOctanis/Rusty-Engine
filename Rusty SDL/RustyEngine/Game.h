#ifndef RUSTYENGINE_GAME_H
#define RUSTYENGINE_GAME_H

#pragma once

#include <SDL.h>
#include "World.h"

namespace RustyEngine
{
	class Game
	{
	private:
		static Uint32 max_lthreads;		// Number of avalible logical threads (at least 1)

		static void init_SDL();
		static bool verCheck();			// TRUE if major versions match | FALSE if minor version mismatches
		static void queryCpuInfo();
		static void quit();				// Clean up all initialized subsystems and SDL
	public:
		static World world;
		static void init();
		static void render();

	private:
		Game();
		~Game();
	};

}

#endif // RUSTYENGINE_GAME_H