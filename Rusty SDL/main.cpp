#if defined _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS		// SDL's C code will trigger warnings otherwise
	#define _RUSTY_WINDOWS_BUILD
	#if _MSC_VER<1800
		#error Must compile with MSVC++ version 18 or newer (vs2013)
	#endif
#else
	#error Must compile with MSVC++ version 18 or newer (vs2013)
#endif


//dev_y
#include <iostream>
#include <SDL.h>
#include <SDL_version.h>
// TO-DO link and import other development libs (mixer, image, ...)

#include <cmath>
#include <string>

// Use this libraries for DX11 gpu computations
//#include <amp.h>
//#include <amp_math.h>

// Internal headers (engine part)
#include "Error.h"
#include "Sprite.h"
#include "Vec2.h"

// TODO - create independed renderer class

// Window and render size limits (currently also settings)
#define WINDOW_MIN_X 320
#define WINDOW_MIN_Y 240
#define RENDER_MIN_X 320
#define RENDER_MIN_Y 240
#define LOCK_LIMIT 10000

using namespace std;
//using namespace concurrency;  // For use with amp


SDL_Window *window;			// Main window pointer
SDL_Renderer *renderer;		// Main renderer pointer
Uint32 max_lthreads = 1;	// Number of avalible logical threads (at least 1)

// TODO - transfer to physics module
float t_delta = 0;			// Time since last frame in ms
float t_scale = 2.0e15f;		// Time scale (changes t_delta directly)

bool buf_swap = false;		//redundant
//bool doAMP = true;		//redundant

// TODO - move SDL inits away from main.cpp
bool verCheck() // TRUE if major versions match | FALSE if minor version mismatches
{
	// Does not report patch version mismatch
	SDL_version compiled;
	SDL_version linked;

	SDL_VERSION(&compiled);		// Get compiled version (set at compile-time)
	SDL_GetVersion(&linked);	// Get linked library version (set at run-time)

								// Display both versions
	cout << "Compiled with SDL version " << +compiled.major << "." << +compiled.minor << "." << +compiled.patch << endl;
	cout << "Linking with SDL version " << +linked.major << "." << +linked.minor << "." << +linked.patch << endl;

	// Major version mismatch or linked minor version lower than compiled version
	// Cannot continue, functions may not work or exist -> crash: SDL_VERSION_MISMATCH
	if (compiled.major != linked.major || compiled.minor > linked.minor)
		exit(SDL_VERSION_MISMATCH);

	// If newer minor version (display warning, but still continue)
	if (compiled.minor < linked.minor)
	{
		cerr << "Linking newer SDL version than intended. Program may not work correctly." << endl;
		return false;
	}
	return true;
}

// Clean up all initialized subsystems and SDL
void quit()
{
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

void init_SDL()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		// Initialization failed -> crash: SDL_INIT_ERROR
		cerr << "SDL_Init Error: " << SDL_GetError() << endl;
		exit(SDL_INIT_ERROR);
	}

	// Create main window at screen center with given resolution
	window = SDL_CreateWindow("Milijon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_MIN_X, WINDOW_MIN_Y, 0);

	if (window == nullptr)
		exit(SDL_INIT_ERROR);
	// TODO replace -1 with renderer selection (get render drivers stuff) in case some optimus laptop setup runs this
	// renderer selection should be made with software renderer or console window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == nullptr)
		exit(SDL_INIT_ERROR);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // Make scaled rendering look smoother (blurred).
	SDL_RenderSetLogicalSize(renderer, RENDER_MIN_X, RENDER_MIN_Y); // Set render size
																	// If render size does not equal window size it will be automaticaly scaled to window size at render-time (each frame)	
}

void queryCpuInfo()
{
	// Number of avalible logical threads (not physical cores)
	max_lthreads = SDL_GetCPUCount();

	// In case SDL goes ape-shit
	if (max_lthreads < 1)
		max_lthreads = 1;

	// Display rest of system capabilities but do nothing with them for now
	cout << "Total system memory: " << SDL_GetSystemRAM() << " MB" << endl;
	cout << "CPU logical threads: " << max_lthreads << endl;
	cout << "CPU L1 cache size: " << SDL_GetCPUCacheLineSize() << " K" << endl;
	cout << "Instruction set support" << endl;
	cout << "-----------------------" << endl;

	if (SDL_HasSSE())
		cout << "SSE: Yes" << endl;
	else
		cout << "SSE: No" << endl;

	if (SDL_HasSSE2())
		cout << "SSE2: Yes" << endl;
	else
		cout << "SSE2: No" << endl;

	if (SDL_HasSSE3())
		cout << "SSE3: Yes" << endl;
	else
		cout << "SSE3: No" << endl;

	if (SDL_HasSSE41())
		cout << "SSE4.1: Yes" << endl;
	else
		cout << "SSE4.1: No" << endl;

	if (SDL_HasSSE42())
		cout << "SSE4.2: Yes" << endl;
	else
		cout << "SSE4.2: No" << endl;

	if (SDL_HasMMX())
		cout << "MMX: Yes" << endl;
	else
		cout << "MMX: No" << endl;

	if (SDL_Has3DNow())
		cout << "3DNow: Yes" << endl;
	else
		cout << "3DNow: No" << endl;

	if (SDL_HasAltiVec())
		cout << "AltiVec: Yes" << endl;
	else
		cout << "AltiVec: No" << endl;

	if (SDL_HasAVX())
		cout << "AVX: Yes" << endl;
	else
		cout << "AVX: No" << endl;

	if (SDL_HasAVX2()) // upcoming in SDL 2.0.4
		cout << "AVX2: Yes" << endl;
	else
		cout << "AVX2: No" << endl;

}

// Load bmp image to SDL_Surface and return it
SDL_Surface* imgLoad(string file_name)
{
	SDL_Surface* output = nullptr;

	output = SDL_LoadBMP(file_name.c_str());

	if (output == nullptr)
		exit(SDL_TEXTURE_ERROR);

	return output;
}

// Main function
int main(int argc, char**argv)
{
	atexit(quit); // This function will be called at exit(); function call. Use for clean up.

	// Check linked sdl version  
	verCheck();

	// Init SDL, window and renderer
	init_SDL();

	// Must run after init(); to get system info
	queryCpuInfo();

	// Must run after queryCpuInfo()
	//framebuffer_test_loop();
	//simulation_main();


	// EVERYTHING UNDERNEATH IS NOT ENGINE RELATED CODE AND CAN, AS WELL AS SHOULD, BE IGNORED
	// New ugly quick crunch code for TINR homeworks here (clean it up soon) /// I MEAN IT YAHARA.... DO IT SOON FFS
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_Surface* testis = imgLoad("sprite.bmp");
	SDL_Surface* screen = SDL_GetWindowSurface(window);
	
	SDL_Rect *dest_rect = new SDL_Rect();
	dest_rect->h = 32;
	dest_rect->w = 32;
	dest_rect->x = 0;
	dest_rect->y = 0;

	SDL_Rect *src_rect = new SDL_Rect();
	src_rect->h = 32;
	src_rect->w = 32;
	src_rect->x = 0;
	src_rect->y = 0;


	SDL_Texture* texture;
	texture = SDL_CreateTextureFromSurface(renderer, testis);
	if (texture == 0) {
		exit(113);
	}
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	//SDL_BlitSurface(testis, NULL, screen, NULL);
	//SDL_BlitScaled(testis, NULL, screen, a);

	SDL_Event event;
	bool done = false;

	Uint32 t_curr = 0;
	Uint32 t_last = SDL_GetTicks();
	Uint32 t_render = 0;
	Uint32 t_total = 0;
	
	float pos_x = 0;
	float pos_y = 0;
	Uint32 mouse_state;
	int mouse_x, mouse_y;
	bool not_released = false;

	while (!done) 
	{
		if (not_released)
		{
			dest_rect->x = mouse_x;
			dest_rect->y = mouse_y;
			SDL_RenderCopy(renderer, texture, src_rect, dest_rect);
			SDL_RenderPresent(renderer);
		}
		if (SDL_PollEvent(&event))	// Grab input events
		{
			// ... don't ask
			if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_FINGERMOTION || event.type == SDL_FINGERDOWN || not_released) // input move event
			{
				mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);		// get mouse location
				//SDL_GetRelativeMouseState(&dx, &dy);	// find how much the mouse moved
				
				if (mouse_state & SDL_BUTTON_LMASK)			// is the mouse (touch) down?
				{
					//drawLine(renderer, x - dx, y - dy, dx, dy);	// draw line segment
					not_released = true;
				}
			}
			if (event.type == SDL_QUIT)
			{
				done = true;
			}
			if (event.type == SDL_MOUSEBUTTONUP || event.type == SDL_FINGERUP)
				not_released = false;
		}

		t_curr = SDL_GetTicks();
		t_delta = (float)(t_curr - t_last) / 1000.0f;
		//cout << t_delta << endl;
		t_last = t_curr;

		pos_x += 40.0f * t_delta;
		pos_y += 60.0f * t_delta;

		// Move sprite
		dest_rect->x = roundf(pos_x);
		dest_rect->y = roundf(pos_y);

		// Render sprite
		SDL_RenderCopy(renderer, texture, src_rect, dest_rect);

		// Display render
		SDL_RenderPresent(renderer);

		t_render = SDL_GetTicks() - t_last;
		if(t_render < 16)	// Almost 60 fps
			SDL_Delay(16 - t_render); // Sleep until next frame

		SDL_RenderClear(renderer);
	}

	//    SDL_FreeSurface(testis);
	/*
	testis = imgLoad("sprite2.bmp");
	a->x = 20;
	a->y = 120;
	a->h = 64;
	a->w = 64;

	SDL_BlitScaled(testis, nullptr, screen, a);
	//SDL_BlitSurface(testis, nullptr, screen, nullptr);
	//SDL_BlitSurface(testis, nullptr, screen, a);

	SDL_Rect b;
	b.h = 100;
	b.w = 100;
	b.x = 0;
	b.y = 0;

	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
	*/
//	SDL_UpdateWindowSurface(window);
	//SDL_Delay(5000);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(testis);
	delete dest_rect;
	delete src_rect;
	SDL_Log("End\n");

	/*
	// Wait before exiting
#if defined _RUSTY_WINDOWS_BUILD
	system("pause");
#else
	SDL_Delay(500);
#endif
*/
	return EXIT_SUCCESS;
}