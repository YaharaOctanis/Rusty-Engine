#include "World.h"
#include "Error.h"
#include "RustyConstants.h"
#include <iostream>

namespace RustyEngine
{
	// Constructors
	World::World()
	{
		name = "World";
		levels.clear();
		active_camera = nullptr;
		audio_listener = nullptr;
		main_renderer = nullptr;
		main_window = nullptr;
	}

	World::World(string name)
	{
		this->name = name;
		levels.clear();
		active_camera = nullptr;
		audio_listener = nullptr;
		main_renderer = nullptr;
		main_window = nullptr;
	}


	// Destructor
	World::~World()
	{
	}


	// Create window and SDL renderer
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


	// Update every level
	void World::update()
	{
		for (size_t i = 0; i < levels.size(); i++)
		{
			if(levels[i]->active)
				levels[i]->update();
		}
	}



	Sprite * World::getSpriteByFilename(string file_name)
	{
		for (unsigned int i = 0; i < sprites.size(); i++)
		{
			if (sprites[i]->file == file_name)
				return sprites[i];
		}
		return nullptr;
	}


	// Transform vector from screen space to world space
	Vec2 World::screenToWorldSpace(Vec2 * a)
	{
		// Cannot transform if we have no active camera or if input vector is null, return vec2(0, 0)
		if (active_camera == nullptr || a == nullptr)
			return Vec2();

		int w, h;
		SDL_RenderGetLogicalSize(main_renderer, &w, &h);	// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(main_renderer, &w, &h);	// get screen size instead

		Vec2 output;

		//Screen to world formula: wx = (dx + cx * 32 * RS - w/2) / (32 * RS)
		//						   wy = -(dx - cx * 32 * RS - w/2) / (32 * RS)
		output.x = (a->x + (active_camera->transform.position.x * 32 * RENDER_SCALE) - w / 2.0f) / (32 * RENDER_SCALE);
		output.y = -(a->y - (active_camera->transform.position.y * 32 * RENDER_SCALE) - h / 2.0f) / (32 * RENDER_SCALE);

		return output;
	}

	Vec2 World::screenToWorldSpace(const Vec2 & a)
	{
		// Cannot transform if we have no active camera or if input vector is null, return vec2(0, 0)
		if (active_camera == nullptr)
			return Vec2();

		int w, h;
		SDL_RenderGetLogicalSize(main_renderer, &w, &h);	// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(main_renderer, &w, &h);	// get screen size instead

		Vec2 output;

		//Screen to world formula: wx = (dx + cx * 32 * RS - w/2) / (32 * RS)
		//						   wy = -(dx - cx * 32 * RS - w/2) / (32 * RS)
		output.x = (a.x + (active_camera->transform.position.x * 32 * RENDER_SCALE) - w / 2.0f) / (32 * RENDER_SCALE);
		output.y = -(a.y - (active_camera->transform.position.y * 32 * RENDER_SCALE) - h / 2.0f) / (32 * RENDER_SCALE);

		return output;
	}


	// Transform screen position from absolute to relative (-1 to 1, where 0 is center) (-1, -1 is bottom left)
	Vec2 World::screenAbsoluteToRelative(Vec2 * a)
	{
		int w, h;
		SDL_RenderGetLogicalSize(main_renderer, &w, &h);	// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(main_renderer, &w, &h);	// get screen size instead
		
		Vec2 output;

		output.x = ((a->x / w) * 2) - 1;
		output.y = ((a->y / h) * -2) + 1;

		return output;
	}


	// Transform screen position from relative to absolute (0, 0 is top left)
	Vec2 World::screenRelativeToAbsolute(Vec2 * a)
	{
		int w, h;
		SDL_RenderGetLogicalSize(main_renderer, &w, &h);	// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(main_renderer, &w, &h);	// get screen size instead

		Vec2 output;

		//(w*(x+1))/2 = ax
		output.x = roundf((a->x + 1) * w) / 2;
		output.y = roundf((a->y - 1) * h) / -2;

		return output;
	}
}