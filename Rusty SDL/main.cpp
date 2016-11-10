#if defined _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#if _MSC_VER<1800
#error Must compile with MSVC++ version 18 or newer (vs2013)
#endif
#else
#error Must compile with MSVC++ version 18 or newer (vs2013)
#endif

#include <iostream>
#include <SDL.h>
// TO-DO link and import other development libs (mixer, image, ...)
#include <SDL_version.h>
#include <cmath>
//#include <amp.h>
//#include <amp_math.h>
#include <string>

// Window and render size limits (currently also settings)
#define WINDOW_MIN_X 320
#define WINDOW_MIN_Y 240
#define RENDER_MIN_X 320
#define RENDER_MIN_Y 240
#define LOCK_LIMIT 10000

// Error codes
#define SDL_VERSION_MISMATCH 200
#define SDL_INIT_ERROR 201
#define SDL_TEXTURE_ERROR 202
#define SDL_THREAD_ERROR 203

using namespace std;
//using namespace concurrency;

SDL_Window *window;			// Main window pointer
SDL_Renderer *renderer;		// Main renderer pointer
Uint32 max_lthreads = 1;	// Number of avalible logical threads (at least 1)
float t_delta = 0;			// Time since last frame in ms
float t_scale = 2.0e15f;		// Time scale (changes t_delta directly)
bool buf_swap = false;

bool doAMP = true;


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
		cerr << "Running newer SDL version than intended. Program may not work correctly." << endl;
		return false;
	}
	return true;
}

// Clean up all initialized subsystems
void quit()
{
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

void init()
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

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // Make scaled rendering look smoother.
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

	//max_lthreads = 1;

	// Display rest of system capabilities but do nothing with them
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
	atexit(quit);
	// Check linked sdl version
	verCheck();

	// Init SDL, window and renderer
	init();

	// Must run after init()
	queryCpuInfo();

	//lottery_start();
	// Must run after queryCpuInfo()
	//framebuffer_test_loop();
	//simulation_main();
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


// TODO: random starting velocity, AMP implementation, acceleration structure, optimization









/*

struct buffer_manifest		// Data send to partial_loader thread
{
	Uint32 *framebuffer;	// Shared access array pointer
	Uint32 *clearbuffer;	// Shared access array pointer
	Uint32 limit;			// Number of items we can access
	Uint32 start_loc;		// Starting index of our items
	Uint32 offset;
	Uint32 star_limit;
	Uint32 star_start;
};

buffer_manifest *thr_data; //= new buffer_manifest[max_lthreads];

// Threaded framebuffer manipulation
int partial_loader(void* data)
{
	// Static cast buffer manifest from void data pointer 
	// Static cast (compile-time exec) has less overhead and runs faster than dynamic cast(run-time exec)
	buffer_manifest *tr_data = static_cast<buffer_manifest*>(data);
	//buffer_manifest manifest = (buffer_manifest)tr_data;

	// Calculate starting x and y from starting location and render width
	unsigned int y = tr_data->start_loc / RENDER_MIN_X;
	unsigned int x = tr_data->start_loc % RENDER_MIN_X;

	// Calculate final index+1
	Uint32 end = tr_data->start_loc + tr_data->limit;

	// On first loop, do not reset x or y
	bool first = true;

	Uint32 temp = tr_data->offset; // Current starting color

	for (; y < RENDER_MIN_Y; y++)
	{
		if (!first)
			x = 0;
		else
			first = false;
		for (; x < RENDER_MIN_X; x++)
		{
			unsigned int n = x + y * RENDER_MIN_X;
			if (n >= end)
				return 0;

			//starlist[n].attract(&blackhole); // calculate star attraction forces
			//tr_data->clearbuffer[n] = 0xff000000; // reset color at prev. position in clearbuffer
			//starlist[n].move(t_delta/1000); // recalculate position
			//tr_data->framebuffer[n] = starlist[n].drawValue(); // write color in framebuffer at new position

			// Write color data into software framebuffer
			tr_data->framebuffer[x + y * RENDER_MIN_X] = temp;

			if (temp == 0xff000000)
				temp = 0xffffffff;
			else
				temp -= 0x00010101;
		}
		temp = tr_data->offset;
	}

	return 0;
}

// Prepare threads for framebuffer loading
void load_framebuffer(Uint32 *framebuffer, Uint32 offset)
{
	for (Uint32 i = 0; i < max_lthreads; i++)
	{
		thr_data[i].offset = offset;

		// Create and start threads
		thr_handlers[i] = SDL_CreateThread(partial_loader, "PartialLoader", &thr_data[i]);
		if (thr_handlers[i] == nullptr)	// Thread creation went to shit -> crash: SDL_THREAD ERROR
			exit(SDL_THREAD_ERROR);
	}

	// Wait until all threads finish their work, then destroy them
	for (Uint32 i = 0; i < max_lthreads; i++)
		SDL_WaitThread(thr_handlers[i], NULL);
}
*/




//struct star_manifest		// Data send to star_update thread
//{
//	Uint32 *framebuffer;	// Shared access array pointer
//	Uint32 *clearbuffer;	// Shared access array pointer
//	bool *lockbuffer;		// Memory locks
//	Uint32 star_limit;
//	Uint32 star_start;
//};
//
//
//void doAMPmagic()
//{
//	array_view<Star, 1> a(NUM_OF_STARS, starlist);
//	array_view<unsigned int, 2> b(NUM_OF_STARS, NUM_OF_STARS);
//
//	//a.discard_data();
//	parallel_for_each(b.extent, [=](index<2> id) restrict(amp)
//	{
//		if (id[0] != id[1])
//		{
//			float x_dist = (a[id[1]].x - a[id[0]].x) * LyToEm;
//			float y_dist = (a[id[1]].y - a[id[0]].y) * LyToEm;
//			float z_dist = (a[id[1]].z - a[id[0]].z) * LyToEm;
//
//			float distance = fast_math::sqrt(fast_math::powf(x_dist, 2) + fast_math::powf(y_dist, 2) + fast_math::powf(z_dist, 2));
//
//			if (distance < 60.0 * LyToEm)
//				return;
//
//			x_dist /= distance;
//			y_dist /= distance;
//			z_dist /= distance;
//
//			//  m1 * m2
//			// --------- * G
//			//	dist^2
//			float force = (a[id[0]].sqrtGmass / distance) * (a[id[1]].sqrtGmass / distance);
//
//			a[id[0]].force_x += (x_dist * force);
//			a[id[0]].force_y += (y_dist * force);
//			a[id[0]].force_z += (z_dist * force);
//			//that->addForce(-x_dist * force, -y_dist * force, -z_dist * force);
//
//			//b[1].attract(&a[idx]);
//		}
//	});
//
//	//copy(a, starlist);
//}
//
//
//int simulate(void* data)
//{
//	// Static cast buffer manifest from void data pointer 
//	// Static cast (compile-time exec) has less overhead and runs faster than dynamic cast(run-time exec)
//	star_manifest *tr_data = static_cast<star_manifest*>(data);
//
//	Uint32 end = tr_data->star_start + tr_data->star_limit;
//	Uint32 tlock = 0;
//	Uint32 pos = 0;
//
//
//	for (unsigned int i = tr_data->star_start; i < end; i++)
//	{
//		// Don't bother locking clearbuffer, since we will write same value everytime
//
//		if (!doAMP)
//		{
//			for (unsigned int j = 0; j < NUM_OF_STARS; j++)
//			{
//				if (i == j)
//					continue;
//				starlist[i].attract(&starlist[j]);
//			}
//		}
//
//		if (one_galaxy_mode)
//		{
//			starlist[i].attract(&blackhole);
//			blackhole.attract(&starlist[i]);
//		}
//
//		if (two_galaxy_mode && !special_rand)
//		{
//			starlist[i].attract(&blackhole); // calculate star attraction forces
//			starlist[i].attract(&blackhole2);
//			blackhole.attract(&starlist[i]);
//			blackhole2.attract(&starlist[i]);
//		}
//
//		//starlist[i].attract(&blackhole3);
//
//		if (starlist[i].x >= RENDER_MIN_X - 1 || starlist[i].x < 0 ||
//			starlist[i].y >= RENDER_MIN_Y - 1 || starlist[i].y < 0)
//		{
//			starlist[i].move(static_cast<float>(t_delta) / 1000); // recalculate position
//			continue;
//		}
//
//		pos = static_cast<Uint32>(roundf(starlist[i].x) + (roundf(starlist[i].y) * RENDER_MIN_X));
//		if (buf_swap)
//			tr_data->framebuffer[pos] = 0xff000000; // reset color at prev. position in clearbuffer
//		else
//			tr_data->clearbuffer[pos] = 0xff000000; // reset color at prev. position in clearbuffer
//
//		starlist[i].move(static_cast<float>(t_delta) / 1000); // recalculate position
//
//		if (starlist[i].x >= RENDER_MIN_X - 1 || starlist[i].x < 0 ||
//			starlist[i].y >= RENDER_MIN_Y - 1 || starlist[i].y < 0)
//		{
//			if (bounce_mode)
//			{
//				float tv;
//				if (starlist[i].x >= RENDER_MIN_X - 1 || starlist[i].x < 0)
//				{
//					tv = starlist[i].vel_x;
//					starlist[i].nvel_x = -starlist[i].vel_x / 2;
//					starlist[i].vel_x = -tv / 2;
//				}
//				if (starlist[i].y >= RENDER_MIN_Y - 1 || starlist[i].y < 0)
//				{
//					tv = starlist[i].vel_y;
//					starlist[i].nvel_y = -starlist[i].vel_y / 2;
//					starlist[i].vel_y = -tv / 2;
//				}
//			}
//			continue;
//		}
//
//		pos = static_cast<int>(roundf(starlist[i].x) + (roundf(starlist[i].y) * RENDER_MIN_X));
//
//		// Wait until you get framebuffer access
//		tlock = 0;
//		while (tr_data->lockbuffer[pos])	// Wait if access is locked
//		{
//			if (tlock > LOCK_LIMIT)
//				break;
//			tlock++;
//		}
//		// Lock framebuffer access
//		tr_data->lockbuffer[pos] = true;
//		if (buf_swap)
//			tr_data->clearbuffer[pos] = starlist[i].drawValue(color_mode); // write color in framebuffer at new position
//		else
//			tr_data->framebuffer[pos] = starlist[i].drawValue(color_mode); // write color in framebuffer at new position
//		// Unlock framebuffer access
//		tr_data->lockbuffer[pos] = false;
//	}
//
//	return 0;
//}
//
//
//// Thread data and thread handler pointers
//star_manifest *star_data; //= new star_manifest[max_lthreads];
//SDL_Thread **thr_handlers; //= new SDL_Thread*[max_lthreads];
//
//
//// Prepare threads for simulation
//void load_threads()
//{
//	if (doAMP)
//		doAMPmagic();
//
//	for (Uint32 i = 0; i < max_lthreads; i++)
//	{
//		// Create and start threads
//		thr_handlers[i] = SDL_CreateThread(simulate, "Simulation", &star_data[i]);
//		if (thr_handlers[i] == nullptr)	// Thread creation went to shit -> crash: SDL_THREAD ERROR
//			exit(SDL_THREAD_ERROR);
//	}
//
//	// Wait until all threads finish their work, then destroy them
//	for (Uint32 i = 0; i < max_lthreads; i++)
//		SDL_WaitThread(thr_handlers[i], NULL);
//
//	if (one_galaxy_mode && !special_rand)
//	{
//		Uint32 pos;
//		blackhole.attract(&blackhole2);
//		pos = static_cast<Uint32>(roundf(blackhole.x) + (roundf(blackhole.y) * RENDER_MIN_X));
//		if (!blackhole_line)
//		{
//			if (buf_swap)
//				star_data[0].framebuffer[pos] = 0xff000000; // reset color at prev. position in clearbuffer
//			else
//				star_data[0].clearbuffer[pos] = 0xff000000; // reset color at prev. position in clearbuffer
//		}
//
//		blackhole.move(static_cast<float>(t_delta) / 1000);
//
//		pos = static_cast<Uint32>(roundf(blackhole.x) + (roundf(blackhole.y) * RENDER_MIN_X));
//
//		if (blackhole_line)
//		{
//			star_data[0].framebuffer[pos] = 0xffffffff;
//			star_data[0].clearbuffer[pos] = 0xffffffff;
//		}
//		else
//		{
//			if (!buf_swap)
//				star_data[0].framebuffer[pos] = 0xffffffff;
//			else
//				star_data[0].clearbuffer[pos] = 0xffffffff;
//		}
//	}
//
//	// Draw blackholez
//	if (two_galaxy_mode && !special_rand)
//	{
//		Uint32 pos;
//		Uint32 pos2;
//		blackhole.attract(&blackhole2);
//		//blackhole.attract(&blackhole3);
//		blackhole2.attract(&blackhole);
//		//blackhole2.attract(&blackhole3);
//		//blackhole3.attract(&blackhole2);
//		//blackhole3.attract(&blackhole);
//		pos = static_cast<Uint32>(roundf(blackhole.x) + (roundf(blackhole.y) * RENDER_MIN_X));
//		pos2 = static_cast<Uint32>(roundf(blackhole2.x) + (roundf(blackhole2.y) * RENDER_MIN_X));
//		if (!blackhole_line)
//		{
//			if (buf_swap)
//			{
//				star_data[0].framebuffer[pos] = 0xff000000; // reset color at prev. position in clearbuffer
//				star_data[0].framebuffer[pos2] = 0xff000000; // reset color at prev. position in clearbuffer
//			}
//			else
//			{
//				star_data[0].clearbuffer[pos] = 0xff000000; // reset color at prev. position in clearbuffer
//				star_data[0].clearbuffer[pos2] = 0xff000000; // reset color at prev. position in clearbuffer
//			}
//		}
//
//		blackhole.move(static_cast<float>(t_delta) / 1000);
//		blackhole2.move(static_cast<float>(t_delta) / 1000);
//
//		pos = static_cast<Uint32>(roundf(blackhole.x) + (roundf(blackhole.y) * RENDER_MIN_X));
//		pos2 = static_cast<Uint32>(roundf(blackhole2.x) + (roundf(blackhole2.y) * RENDER_MIN_X));
//		if (blackhole_line)
//		{
//			star_data[0].framebuffer[pos] = 0xffffffff;
//			star_data[0].framebuffer[pos2] = 0xffffffff;
//			star_data[0].clearbuffer[pos] = 0xffffffff;
//			star_data[0].clearbuffer[pos2] = 0xffffffff;
//		}
//		else
//		{
//			if (!buf_swap)
//			{
//				star_data[0].framebuffer[pos] = 0xffffffff;
//				star_data[0].framebuffer[pos2] = 0xffffffff;
//			}
//			else
//			{
//				star_data[0].clearbuffer[pos] = 0xffffffff;
//				star_data[0].clearbuffer[pos2] = 0xffffffff;
//			}
//		}
//		//blackhole3.move(static_cast<float>(t_delta) / 1000);
//	}
//}
//
//
//void simulation_main()
//{
//	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Redraw color (clear color)
//	SDL_RenderClear(renderer);						// Clear render buffer
//	SDL_RenderPresent(renderer);					// Display render buffer
//
//	// Create main display texture
//	SDL_Texture *main_texture = nullptr;
//	main_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, RENDER_MIN_X, RENDER_MIN_Y);
//
//	// Could not create display texture -> crash: SDL_TEXTURE_ERROR
//	if (main_texture == nullptr)
//		exit(SDL_TEXTURE_ERROR);
//
//	// Create software framebuffer
//	Uint32 *framebuffer = new Uint32[RENDER_MIN_X*RENDER_MIN_Y];
//	Uint32 *clearbuffer = new Uint32[RENDER_MIN_X*RENDER_MIN_Y];
//	bool *lockbuffer = new bool[RENDER_MIN_X*RENDER_MIN_Y];
//	//Uint32 *tempbuffer = nullptr;
//
//	// Reset all buffers
//	for (int i = 0; i < RENDER_MIN_X * RENDER_MIN_Y; i++)
//	{
//		framebuffer[i] = 0xff000000;
//		clearbuffer[i] = 0xff000000;
//		lockbuffer[i] = false;
//	}
//
//	if (two_galaxy_mode)
//	{
//		Star::randomStart(starlist, NUM_OF_STARS, RENDER_MIN_X / 2, RENDER_MIN_Y, 1024, all_random, special_rand);
//		for (int i = 0; i < NUM_OF_STARS / 2; i++)
//			starlist[i].x += RENDER_MIN_X / 2;
//	}
//	else
//	{
//		Star::randomStart(starlist, NUM_OF_STARS, RENDER_MIN_X, RENDER_MIN_Y, 1024, all_random, special_rand);
//	}
//
//	// Init thread structs
//	star_data = new star_manifest[max_lthreads];
//	thr_handlers = new SDL_Thread*[max_lthreads];
//
//	// Calculate work distribution
//	Uint32 star_split = NUM_OF_STARS / max_lthreads;
//	Uint32 star_remainder = NUM_OF_STARS % max_lthreads;
//
//	// Load thread distribution data
//	for (Uint32 i = 0; i < max_lthreads; i++)
//	{
//		star_data[i].framebuffer = framebuffer;
//		star_data[i].clearbuffer = clearbuffer;
//		star_data[i].lockbuffer = lockbuffer;
//		star_data[i].star_limit = star_split;
//		star_data[i].star_start = star_split * i;
//
//		// Last thread gets extra work if distribution is not round number
//		if (i + 1 >= max_lthreads && star_remainder > 0)
//			star_data[i].star_limit += star_remainder;
//	}
//
//	Uint32 t_last = SDL_GetTicks();	// Grab current time
//	Uint32 t_temp = 0;
//	Uint32 t_total = 0;
//	starlist[0].debug = true;
//	/*
//	for (int i = 0; i < 64000000; i++)
//	{
//	t_total += SDL_GetTicks();
//	}
//	cout << SDL_GetTicks() - t_last << endl;*/
//
//	// Main render loop
//	while (1)
//	{
//		//cout << sqrt(starlist[0].vel_x * starlist[0].vel_x + starlist[0].vel_y * starlist[0].vel_y + starlist[0].vel_z * starlist[0].vel_z) << endl;
//		//cout << sqrt(starlist[0].nvel_x * starlist[0].nvel_x + starlist[0].nvel_y * starlist[0].nvel_y + starlist[0].nvel_z * starlist[0].nvel_z) << endl;
//		//cout << starlist[0].vel_x << " " << starlist[0].vel_y << " " << starlist[0].vel_z << endl;
//		//cout << starlist[0].nvel_x << " " << starlist[0].nvel_y << " " << starlist[0].nvel_z << endl;
//		load_threads();
//		if (buf_swap)
//			SDL_UpdateTexture(main_texture, NULL, clearbuffer, RENDER_MIN_X * sizeof(Uint32));
//		else
//			SDL_UpdateTexture(main_texture, NULL, framebuffer, RENDER_MIN_X * sizeof(Uint32));
//
//		//SDL_RenderClear(renderer);
//		SDL_RenderCopy(renderer, main_texture, NULL, NULL);
//		SDL_RenderPresent(renderer);
//
//		// Swap frame and clear buffers
//		//tempbuffer = framebuffer;
//		//framebuffer = clearbuffer;
//		//clearbuffer = tempbuffer;
//		//tempbuffer = nullptr;
//		if (!line_mode)
//			buf_swap = !buf_swap;
//
//		//cout << SDL_GetTicks() - t_last << endl;
//		SDL_PumpEvents();
//		t_temp = SDL_GetTicks();
//		t_delta = (t_temp - t_last) * t_scale;
//		t_last = t_temp;
//		//t_total += t_delta;
//	}
//
//	// Output average frame time
//	//cout << "Average delta time: " << (float)(t_total) / 200 << endl;
//
//	// Free main texture from memory and free rest of the memory
//	SDL_DestroyTexture(main_texture);
//	delete[] framebuffer;
//	delete[] clearbuffer;
//	delete[] lockbuffer;
//	delete[] star_data;
//	delete[] thr_handlers;
//	//delete main_texture;
//}

//void clear_buffer(Uint32 *buffer, int size, Uint32 value = 0xff000000)
//{
//	for (int i = 0; i < size; i++)
//		buffer[i] = value;
//}
//
//void simulation_main()
//{
//	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Redraw color (clear color)
//	SDL_RenderClear(renderer);						// Clear render buffer
//	SDL_RenderPresent(renderer);					// Display render buffer
//
//													// Create main display texture
//	SDL_Texture *main_texture = nullptr;
//	main_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, RENDER_MIN_X, RENDER_MIN_Y);
//
//	// Could not create display texture -> crash: SDL_TEXTURE_ERROR
//	if (main_texture == nullptr)
//		exit(SDL_TEXTURE_ERROR);
//
//	// Create software framebuffer
//	Uint32 *framebuffer = new Uint32[RENDER_MIN_X*RENDER_MIN_Y];
//	Uint32 *clearbuffer = new Uint32[RENDER_MIN_X*RENDER_MIN_Y];
//	bool *lockbuffer = new bool[RENDER_MIN_X*RENDER_MIN_Y];
//	//Uint32 *tempbuffer = nullptr;
//
//	// Reset all buffers
//	for (int i = 0; i < RENDER_MIN_X * RENDER_MIN_Y; i++)
//	{
//		framebuffer[i] = 0xff000000;
//		clearbuffer[i] = 0xff000000;
//		lockbuffer[i] = false;
//	}
//
//	// Init thread structs
//	//star_data = new star_manifest[max_lthreads];
//	//thr_handlers = new SDL_Thread*[max_lthreads];
//
//	// Calculate thread work distribution
//	//Uint32 star_split = NUM_OF_STARS / max_lthreads;
//	//Uint32 star_remainder = NUM_OF_STARS % max_lthreads;
//
//	// Load thread distribution data
//	//for (Uint32 i = 0; i < max_lthreads; i++)
//	//{
//	//	star_data[i].framebuffer = framebuffer;
//	//	star_data[i].clearbuffer = clearbuffer;
//	//	star_data[i].lockbuffer = lockbuffer;
//	//	star_data[i].star_limit = star_split;
//	//	star_data[i].star_start = star_split * i;
//
//	//	// Last thread gets extra work if distribution is not round number
//	//	if (i + 1 >= max_lthreads && star_remainder > 0)
//	//		star_data[i].star_limit += star_remainder;
//	//}
//
//	Uint32 t_last = SDL_GetTicks();	// Grab current time
//	Uint32 t_temp = 0;
//	Uint32 t_total = 0;
//
//	// Main render loop
//	while (1)
//	{
//		//load_threads();
//
//		// Prepare framebuffer
//
//
//		// Render framebuffer
//		SDL_UpdateTexture(main_texture, NULL, framebuffer, RENDER_MIN_X * sizeof(Uint32));
//
//		// Clear framebuffer to 0xff000000
//		clear_buffer(framebuffer, RENDER_MIN_X * RENDER_MIN_Y);
//
//		/*
//		if (buf_swap)
//		SDL_UpdateTexture(main_texture, NULL, clearbuffer, RENDER_MIN_X * sizeof(Uint32));
//		else
//		SDL_UpdateTexture(main_texture, NULL, framebuffer, RENDER_MIN_X * sizeof(Uint32));
//		*/
//		//SDL_RenderClear(renderer);
//		SDL_RenderCopy(renderer, main_texture, NULL, NULL);
//		SDL_RenderPresent(renderer);
//
//		// Swap frame and clear buffers
//		//tempbuffer = framebuffer;
//		//framebuffer = clearbuffer;
//		//clearbuffer = tempbuffer;
//		//tempbuffer = nullptr;
//
//		// Buffer swap
//		//buf_swap = !buf_swap;
//
//		//cout << SDL_GetTicks() - t_last << endl;
//		SDL_PumpEvents();
//		t_temp = SDL_GetTicks();
//		t_delta = (t_temp - t_last) * t_scale;
//		t_last = t_temp;
//		//t_total += t_delta;
//	}
//
//	// Output average frame time
//	//cout << "Average delta time: " << (float)(t_total) / 200 << endl;
//
//	// Free main texture from memory and free rest of the memory
//	SDL_DestroyTexture(main_texture);
//	delete[] framebuffer;
//	delete[] clearbuffer;
//	delete[] lockbuffer;
//	//delete[] star_data;
//	//delete[] thr_handlers;
//	//delete main_texture;
//}
//
//const int NUMBER_POOL = 39;
//const int NUMBERS_PLAYED = 8;
//
//void shuffle_numbers(int *array)
//{
//	long i = 0;
//	long r = 0;
//	long temp = 0;
//
//	for (i = 0; i < NUMBERS_PLAYED; i++)
//	{
//		r = rand() % NUMBER_POOL;
//
//		//printf("%d, %d\n", array[i], array[r]);
//
//		temp = array[i];
//		array[i] = array[r];
//		array[r] = temp;
//	}
//
//}
///*
//int comp(const void * elem1, const void * elem2)
//{
//int f = *((int*)elem1);
//int s = *((int*)elem2);
//if (f > s) return  1;
//if (f < s) return -1;
//return 0;
//}
//*/
//
//long check_numbers(int *picked, int *random, bool *isSelected)
//{
//	int bonus_number_picked = picked[NUMBERS_PLAYED - 1];
//	int bonus_number_random = random[NUMBERS_PLAYED - 1];
//
//	int i = 0;
//
//	int count_matches = 0;
//	int isBonusPicked = 0;
//
//	for (i = 0; i < NUMBERS_PLAYED - 1; i++)
//		isSelected[random[i] - 1] = true;
//
//	for (i = 0; i < NUMBERS_PLAYED - 1; i++)
//	{
//		if (isSelected[picked[i] - 1] == true)
//			count_matches++;
//	}
//
//	for (i = 0; i < NUMBERS_PLAYED - 1; i++)
//	{
//		isSelected[random[i] - 1] = false;
//	}
//
//	/*for (int i = 0; i < NUMBERS_PLAYED - 1; i++)
//	{
//	for (int j = 0; j < NUMBERS_PLAYED - 1; j++)
//	{
//	if (picked[i] == picked[j])
//	count_matches++;
//	}
//	}*/
//
//	if (bonus_number_picked == bonus_number_random)
//		isBonusPicked = 1;
//
//	return count_matches * 10 + isBonusPicked;
//}
//
///*
//int check_numbers_old(int *picked, int *random)
//{
//int bonus_number_picked = picked[NUMBERS_PLAYED - 1];
//int bonus_number_random = random[NUMBERS_PLAYED - 1];
//
//int i = 0;
//int j = 0;
//
//int count_matches = 0;
//int isBonusPicked = 0;
//
//for (i = 0; i < NUMBERS_PLAYED - 1; i++)
//{
//for (j = 0; j < NUMBERS_PLAYED - 1; j++)
//{
//if (picked[i] == random[j])
//{
//count_matches++;
//}
//}
//}
//
//if (bonus_number_picked == bonus_number_random)
//isBonusPicked = 1;
//
//return count_matches * 10 + isBonusPicked;
//}
//*/
//
//long calculate_profit(long correct_picks, long bonus, long *results)
//{
//	long money_made = 0;
//
//	if (correct_picks < 3)
//		return money_made;
//
//	if (bonus == 0)
//	{
//		if (correct_picks < 4)
//			return money_made;
//		switch (correct_picks)
//		{
//			case 4: money_made = 4; results[6]++; break;
//			case 5: money_made = 43; results[4]++; break;
//			case 6: money_made = 824; results[2]++; break;
//			case 7: money_made = 789751; results[0]++; break;
//		}
//	}
//	else
//	{
//		switch (correct_picks)
//		{
//			case 3: money_made = 5; results[7]++; break;
//			case 4: money_made = 11; results[5]++; break;
//			case 5: money_made = 274; results[3]++; break;
//			case 6: money_made = 6047; results[1]++; break;
//		}
//	}
//
//	return money_made;
//}
//
//long output_result(long correct_picks, long bonus, long money_made, long *results)
//{
//	int i = 0;
//
//	if (bonus == 1)
//	{
//		printf("%d + %d : %d eur\n", correct_picks, bonus, money_made);
//	}
//	else
//	{
//		printf("%d : %d eur\n", correct_picks, money_made);
//	}
//
//	for (i = 0; i < 8; i++)
//	{
//		switch (i)
//		{
//			case 0: printf("Stevilo 7: %d\n", results[i]); break;
//			case 1: printf("Stevilo 6+1: %d\n", results[i]); break;
//			case 2: printf("Stevilo 6: %d\n", results[i]); break;
//			case 3: printf("Stevilo 5+1: %d\n", results[i]); break;
//			case 4: printf("Stevilo 5: %d\n", results[i]); break;
//			case 5: printf("Stevilo 4+1: %d\n", results[i]); break;
//			case 6: printf("Stevilo 4: %d\n", results[i]); break;
//			case 7: printf("Stevilo 3+1: %d\n", results[i]); break;
//		}
//	}
//
//	return money_made;
//}
//
//struct game_manifest
//{
//	int id;
//	long money_made;
//	long money_made_this_round;
//	long results[8];
//	int number_pool[NUMBER_POOL];
//	int *picked_numbers;
//	long result;
//	long game_limit;
//	bool is_done;
//}*game_data;
//
////game_data = new game_manifest[max_lthreads];
//SDL_Thread **thr_handlers;
//SDL_TLSID *thr_local_storage;
////// Thread data and thread handler pointers
////star_manifest *star_data; //= new star_manifest[max_lthreads];
////SDL_Thread **thr_handlers; //= new SDL_Thread*[max_lthreads];
//
//int play_lottery(void* data)
//{
//	// Static cast buffer manifest from void data pointer 
//	// Static cast (compile-time exec) has less overhead and runs faster than dynamic cast(run-time exec)
//
//	game_manifest *tr_data = static_cast<game_manifest*>(data);
//	bool *isSelected = new bool[NUMBER_POOL];
//
//	for (int i = 0; i < NUMBER_POOL; i++)
//		isSelected[i] = false;
//
//	//SDL_TLSSet(tr_data->id, data, 0);
//
//	for (int i = 0; i < tr_data->game_limit; i++)
//	{
//		tr_data->money_made_this_round = 0;
//		tr_data->money_made -= 1;
//
//		shuffle_numbers(tr_data->number_pool);
//
//		long result = check_numbers(tr_data->picked_numbers, tr_data->number_pool, isSelected);
//		tr_data->money_made_this_round = calculate_profit(result / 10, result % 10, tr_data->results);
//		tr_data->money_made += tr_data->money_made_this_round;
//	}
//	tr_data->is_done = true;
//	return 0;
//}
//
//int frequency = 0;
//
//int display_lottery(void* data)
//{
//	long results[8];
//	long result;
//	long money_made;
//	bool finish_it;
//
//	//void *s_data;
//	//game_manifest **sim_data;
//	//sim_data = new game_manifest*[max_lthreads];
//	//game_manifest c_data = game_data[0];
//	/*
//	for (int i = 0; i < max_lthreads; i++)
//	{
//	s_data = SDL_TLSGet(game_data[i].id);
//	sim_data[i] = static_cast<game_manifest*>(s_data);
//	}
//	*/
//	int x = 0;
//	do
//	{
//		result = 0;
//		money_made = 0;
//		for (int j = 0; j < 8; j++)
//		{
//			results[j] = 0;
//		}
//
//		finish_it = true;
//		for (Uint32 i = 0; i < max_lthreads; i++)
//		{
//			result += game_data[i].result;
//			money_made += game_data[i].money_made;
//			for (int j = 0; j < 8; j++)
//			{
//				results[j] += game_data[i].results[j];
//			}
//
//			if (!game_data[i].is_done)
//				finish_it = false;
//		}
//
//		system("cls");
//		output_result(result / 10, result % 10, 0, results);
//		printf("Skupen denar: %de\n\n", money_made);
//		SDL_Delay(1000);
//
//	} while (!finish_it);
//
//	return 0;
//}
//
//SDL_Thread *display_thread;
//
//// Prepare threads for simulation
//void load_threads()
//{
//	for (Uint32 i = 0; i < max_lthreads; i++)
//	{
//		// Create and start threads
//		thr_handlers[i] = SDL_CreateThread(play_lottery, "Simulation", &game_data[i]);
//		if (thr_handlers[i] == nullptr)	// Thread creation went to shit -> crash: SDL_THREAD ERROR
//			exit(SDL_THREAD_ERROR);
//	}
//
//	display_thread = SDL_CreateThread(display_lottery, "Display", NULL);
//
//	// Wait until all threads finish their work, then destroy them
//	for (Uint32 i = 0; i < max_lthreads; i++)
//		SDL_WaitThread(thr_handlers[i], NULL);
//	SDL_WaitThread(display_thread, NULL);
//}
//
//
//
//int lottery_start()
//{
//	if (max_lthreads > 1)
//		max_lthreads -= 1;
//	//max_lthreads = 1;
//	long TIMES_PLAYED = 100;
//
//	srand(time(NULL));
//
//	int number_pool[NUMBER_POOL];
//	Uint32 i = 0;
//
//	for (i = 0; i < NUMBER_POOL; i++)
//	{
//		number_pool[i] = i + 1;
//	}
//
//	int picked_numbers[NUMBERS_PLAYED];
//
//	for (i = 0; i < NUMBERS_PLAYED; i++)
//	{
//		if (i == 7)
//			printf("Vnesi bonus stevilko: ");
//		else
//			printf("Vnesi stevilko %d: ", i + 1);
//
//		scanf("%d", &picked_numbers[i]);
//	}
//
//	printf("Kolikokrat bi radi igrali: ");
//	scanf("%d", &TIMES_PLAYED);
//	/*
//	printf("Frekvenca izpisa: ");
//	scanf("%d", &frequency);
//	*/
//	long results[8];
//	long money_made = 0;
//	long money_made_this_round = 0;
//
//	for (i = 0; i < 8; i++)
//	{
//		results[i] = 0;
//	}
//
//	// Init thread structs
//	game_data = new game_manifest[max_lthreads];
//	thr_handlers = new SDL_Thread*[max_lthreads];
//	//thr_local_storage = new SDL_TLSID[max_lthreads];
//
//	// Calculate thread work distribution
//	Uint32 game_split = TIMES_PLAYED / max_lthreads;
//	Uint32 game_remainder = TIMES_PLAYED % max_lthreads;
//
//	// Load thread distribution data
//	for (i = 0; i < max_lthreads; i++)
//	{
//		//game_data[i].id = SDL_TLSCreate();
//		game_data[i].money_made = 0;
//		game_data[i].money_made_this_round = 0;
//		game_data[i].result = 0;
//
//		for (Uint32 j = 0; j < 8; j++)
//			game_data[i].results[j] = 0;
//
//		for (Uint32 j = 0; j < NUMBER_POOL; j++)
//			game_data[i].number_pool[j] = number_pool[j];
//
//		game_data[i].picked_numbers = picked_numbers;
//		game_data[i].game_limit = game_split;
//
//		game_data[i].is_done = false;
//
//		// Last thread gets extra work if distribution is not round number
//		if (i + 1 >= max_lthreads && game_remainder > 0)
//			game_data[i].game_limit += game_remainder;
//	}
//	float t_start = (float)SDL_GetTicks();	// Grab current time
//	load_threads();
//	float t_last = (float)SDL_GetTicks();	// Grab current time
//	system("cls");
//	printf("Time elapsed: %fs", (t_last - t_start) / 1000.0f);
//	/*
//	for (i = 0; i < TIMES_PLAYED; i++)
//	{
//	money_made_this_round = 0;
//	money_made -= 1;
//
//	shuffle_numbers(number_pool);
//
//	int result = check_numbers(picked_numbers, number_pool);
//	money_made_this_round = calculate_profit(result / 10, result % 10, results);
//	money_made += money_made_this_round;
//
//
//	if (i%frequency == 0)
//	{
//	system("cls");
//	output_result(result / 10, result % 10, money_made_this_round, results);
//	printf("Skupen denar: %de\n\n", money_made);
//	}
//
//	}
//	*/
//	long result = 0;
//
//	for (i = 0; i < max_lthreads; i++)
//	{
//		money_made_this_round = 0;
//		result += game_data[i].result;
//		money_made += game_data[i].money_made;
//		for (int j = 0; j < 8; j++)
//			results[j] += game_data[i].results[j];
//	}
//
//	output_result(result / 10, result % 10, money_made_this_round, results);
//	printf("Skupen denar: %d eur\n\n", money_made);
//	//system("PAUSE");
//	return 0;
//}