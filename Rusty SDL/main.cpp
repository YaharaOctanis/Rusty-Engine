#if defined _MSC_VER
#define _CRT_SECURE_NO_WARNINGS		// SDL's C code will trigger warnings otherwise
#if _MSC_VER<1800
#error Must compile with MSVC++ version 18 or newer (vs2013)
#endif
#else
#error Must compile with MSVC++ version 18 or newer (vs2013)
#endif

#include <iostream>
#include <SDL.h>
#include <SDL_version.h>
// TO-DO link and import other development libs (mixer, image, ...)

#include <cmath>
#include <string>

// Use this libraries for DX11 gpu computations
//#include <amp.h>
//#include <amp_math.h>

// TODO - create independed renderer class
// Window and render size limits (currently also settings)
#define WINDOW_MIN_X 320
#define WINDOW_MIN_Y 240
#define RENDER_MIN_X 320
#define RENDER_MIN_Y 240
#define LOCK_LIMIT 10000

// TODO - move error codes to their own file
// Error codes - SDL for Renderer
#define SDL_VERSION_MISMATCH 200
#define SDL_INIT_ERROR 201
#define SDL_TEXTURE_ERROR 202
#define SDL_THREAD_ERROR 203

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

	// New test code for TINR game here (clean it up soon)
	SDL_Surface* testis = imgLoad("sprite.bmp");
	SDL_Surface* screen = SDL_GetWindowSurface(window);
	SDL_Rect *a = new SDL_Rect();
	a->h = 128;
	a->w = 128;
	a->x = 50;
	a->y = 30;

	SDL_BlitSurface(testis, nullptr, screen, nullptr);
	SDL_BlitScaled(testis, nullptr, screen, a);

	SDL_FreeSurface(testis);

	testis = imgLoad("sprite2.bmp");
	a->x = 20;
	a->y = 120;
	a->h = 64;
	a->w = 64;

	SDL_BlitScaled(testis, nullptr, screen, a);
	//SDL_BlitSurface(testis, nullptr, screen, nullptr);
	//SDL_BlitSurface(testis, nullptr, screen, a);
	SDL_UpdateWindowSurface(window);

	SDL_Delay(5000);

	SDL_FreeSurface(testis);
	delete a;
	system("pause");
	return EXIT_SUCCESS;
}