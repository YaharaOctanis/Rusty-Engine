#include "World.h"
#include "Error.h"
#include <iostream>

namespace RustyEngine
{
	World::World()
	{
		name = "world";
		levels.clear();
		active_camera = nullptr;
		main_renderer = nullptr;
		main_window = nullptr;

		//init();
	}

	World::World(string name)
	{
		this->name = name;
		levels.clear();
		active_camera = nullptr;
		main_renderer = nullptr;
		main_window = nullptr;

		//init();
	}

	World::~World()
	{
	}


	void World::init()
	{
		// Create main window at screen center with given resolution
		main_window = SDL_CreateWindow("Rusty Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_MIN_X, WINDOW_MIN_Y, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

		if (main_window == nullptr)
			exit(SDL_INIT_ERROR);

		// Detect and display avalible video drivers
		std::cout << "Number of detected video drivers: " << SDL_GetNumVideoDrivers() << std::endl;

		for (int i = 0; i < SDL_GetNumVideoDrivers(); i++)
			std::cout << "Driver " << i << ": " << SDL_GetVideoDriver(i) << std::endl;

		// TODO replace -1 with renderer selection (get render drivers stuff) in case some optimus laptop setup runs this
		// renderer selection should be made with software renderer or console window
		main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);

		if (main_renderer == nullptr)
			exit(SDL_INIT_ERROR);

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // Make scaled rendering look pixelated/retro.
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
		SDL_RenderSetLogicalSize(main_renderer, RENDER_MIN_X, RENDER_MIN_Y); // Set render size
																		// If render size does not equal window size it will be automaticaly scaled to window size at render-time (each frame)	
		std::cout << "Initialized video driver: " << SDL_GetCurrentVideoDriver() << std::endl;

		int width, height;

		SDL_RenderGetLogicalSize(main_renderer, &width, &height);		// get render target size

		if (width == 0 || height == 0)		// if no render target size
			SDL_GetRendererOutputSize(main_renderer, &width, &height);	// get screen size instead

		//cout << "==============================================" << endl;
		std::cout << " " << name << std::endl;
		std::cout << "==============================================" << std::endl;
		std::cout << "Rendering at: " << width << " x " << height << std::endl;

		std::cout << std::endl;
	}
	void World::update()
	{
		for (int i = 0; i < levels.size(); i++)
		{
			if(levels[i]->active)
				levels[i]->update();
		}
	}
}