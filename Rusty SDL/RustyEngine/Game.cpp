#include "Game.h"
#include <iostream>
#include "Error.h"
#include "Time.h"
#include "Audio.h"

namespace RustyEngine
{
	Uint32 Game::max_lthreads;	// Number of avalible logical threads (at least 1)
	World Game::world;			// Game world

	// Initialize SDL (also create game renderer and system window)
	void Game::init_SDL()
	{
		max_lthreads = 1;

		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			// Initialization failed -> crash: SDL_INIT_ERROR
			std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
			exit(SDL_INIT_ERROR);
		}
	}


	// Check if linked SDL library matches compiled version
	// (TRUE if major versions match | FALSE if minor versions mismatch | ERROR/EXIT if major versions mismatch)
	bool Game::verCheck()
	{
		// Does not report patch version mismatch
		SDL_version compiled;
		SDL_version linked;

		SDL_VERSION(&compiled);		// Get compiled version (set at compile-time)
		SDL_GetVersion(&linked);	// Get linked library version (set at run-time)

		// Display both versions
		std::cout << "Compiled with SDL version " << +compiled.major << "." << +compiled.minor << "." << +compiled.patch << std::endl;
		std::cout << "Linking with SDL version " << +linked.major << "." << +linked.minor << "." << +linked.patch << std::endl << std::endl;

		// Major version mismatch or linked minor version lower than compiled version
		// Cannot continue, functions may not work or exist -> crash: SDL_VERSION_MISMATCH
		if (compiled.major != linked.major || compiled.minor > linked.minor)
			exit(SDL_VERSION_MISMATCH);

		// If newer minor version (display warning, but still continue)
		if (compiled.minor < linked.minor)
		{
			std::cerr << "Linking newer SDL version than intended. Program may not work correctly." << std::endl << std::endl;
			return false;
		}
		return true;
	}


	// Check system capabilities (instruction set support, number of logical threads, cpu speed, etc.)
	void Game::queryCpuInfo()
	{
		// Number of avalible logical threads (not physical cores)
		max_lthreads = SDL_GetCPUCount();

		// In case SDL goes ape-shit (there is always at least one logical thread, otherwise this could not even run)
		if (max_lthreads < 1)
			max_lthreads = 1;

		// Display rest of system capabilities but do nothing with them for now
		std::cout << "Total system memory: " << SDL_GetSystemRAM() << " MB" << std::endl;
		std::cout << "CPU logical threads: " << max_lthreads << std::endl;
		std::cout << "CPU L1 cache size: " << SDL_GetCPUCacheLineSize() << " K" << std::endl;
		std::cout << "Instruction set support" << std::endl;
		std::cout << "-----------------------" << std::endl;

		if (SDL_HasSSE())
			std::cout << "SSE: Yes" << std::endl;
		else
			std::cout << "SSE: No" << std::endl;

		if (SDL_HasSSE2())
			std::cout << "SSE2: Yes" << std::endl;
		else
			std::cout << "SSE2: No" << std::endl;

		if (SDL_HasSSE3())
			std::cout << "SSE3: Yes" << std::endl;
		else
			std::cout << "SSE3: No" << std::endl;

		if (SDL_HasSSE41())
			std::cout << "SSE4.1: Yes" << std::endl;
		else
			std::cout << "SSE4.1: No" << std::endl;

		if (SDL_HasSSE42())
			std::cout << "SSE4.2: Yes" << std::endl;
		else
			std::cout << "SSE4.2: No" << std::endl;

		if (SDL_HasMMX())
			std::cout << "MMX: Yes" << std::endl;
		else
			std::cout << "MMX: No" << std::endl;

		if (SDL_Has3DNow())
			std::cout << "3DNow: Yes" << std::endl;
		else
			std::cout << "3DNow: No" << std::endl;

		if (SDL_HasAltiVec())
			std::cout << "AltiVec: Yes" << std::endl;
		else
			std::cout << "AltiVec: No" << std::endl;

		if (SDL_HasAVX())
			std::cout << "AVX: Yes" << std::endl;
		else
			std::cout << "AVX: No" << std::endl;

		if (SDL_HasAVX2())
			std::cout << "AVX2: Yes" << std::endl;
		else
			std::cout << "AVX2: No" << std::endl;
	}


	// Clean up all initialized subsystems and SDL
	void Game::quit()
	{
		// DO VRAM AND RAM CLEANUP HERE
		// TODO cleanup of all engine subsystems and game objects
		Audio::quit();
		
		SDL_QuitSubSystem(SDL_INIT_EVERYTHING);	// Stop SDL subsystems
		SDL_Quit();								// Stop SDL
	}


	// Initialize game engine
	void Game::init()
	{
		// This function will be called at exit(); function call. Use for clean up.
		atexit(Game::quit); 

		// Check linked sdl version  
		verCheck();

		// Init SDL, window and renderer
		init_SDL();

		// Must run after init_SDL(); to get system info
		queryCpuInfo();

		Audio::init();
		Time::init();
	}


	// Render game world (SDL render command)
	void Game::render()
	{
		SDL_RenderPresent(world.main_renderer);
	}


	// Don't even think about constructing class with only static members
	Game::Game()
	{
	}


	Game::~Game()
	{
	}
}