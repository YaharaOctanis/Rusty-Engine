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
#include <list>
#include <vector>

// Use this libraries for DX11 gpu computations
//#include <amp.h>
//#include <amp_math.h>

// Internal headers (engine part)
#include "RustyEngine/Error.h"
#include "RustyEngine/Sprite.h"
#include "RustyEngine/Vec2.h"
#include "RustyEngine/Component.h"
#include "RustyEngine/GameObject.h"
#include "RustyEngine/Transform.h"
#include "RustyEngine/Renderer.h"
#include "RustyEngine/RustyConstants.h"
#include "RustyEngine/Time.h"

// TODO - create renderer loop class/thingy

using namespace std;
using namespace RustyEngine;
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
	cout << "Linking with SDL version " << +linked.major << "." << +linked.minor << "." << +linked.patch << endl << endl;

	// Major version mismatch or linked minor version lower than compiled version
	// Cannot continue, functions may not work or exist -> crash: SDL_VERSION_MISMATCH
	if (compiled.major != linked.major || compiled.minor > linked.minor)
		exit(SDL_VERSION_MISMATCH);

	// If newer minor version (display warning, but still continue)
	if (compiled.minor < linked.minor)
	{
		cerr << "Linking newer SDL version than intended. Program may not work correctly." << endl << endl;
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
	window = SDL_CreateWindow("Milijon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_MIN_X, WINDOW_MIN_Y, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

	if (window == nullptr)
		exit(SDL_INIT_ERROR);
	
	// Detect and display avalible video drivers
	cout << "Number of detected video drivers: " << SDL_GetNumVideoDrivers() << endl;

	for (int i = 0; i < SDL_GetNumVideoDrivers(); i++)
		cout << "Driver " << i << ": " << SDL_GetVideoDriver(i) << endl;

	// TODO replace -1 with renderer selection (get render drivers stuff) in case some optimus laptop setup runs this
	// renderer selection should be made with software renderer or console window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == nullptr)
		exit(SDL_INIT_ERROR);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // Make scaled rendering look pixelated/retro.
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
	SDL_RenderSetLogicalSize(renderer, RENDER_MIN_X, RENDER_MIN_Y); // Set render size
																	// If render size does not equal window size it will be automaticaly scaled to window size at render-time (each frame)	
	cout << "Initialized video driver: " << SDL_GetCurrentVideoDriver() << endl;

	cout << endl;
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

	int width, height;

	SDL_RenderGetLogicalSize(renderer, &width, &height);		// get render target size

	if (width == 0 || height == 0)		// if no render target size
		SDL_GetRendererOutputSize(renderer, &width, &height);	// get screen size instead

	//cout << "==============================================" << endl;
	cout << " Main Screen" << endl;
	cout << "==============================================" << endl;
	cout << "Rendering at: " << width << " x " << height << endl;
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

/*Ugly game logic here*/
class RoboLogic : public Component
{
public:

	GameObject* game_object;	// reference to parent game object
	GameObject* coin;
	float score = 0;
	Vec2 speed;
	float delta_t;
	RoboLogic(GameObject* g_obj) { game_object = g_obj; }
	~RoboLogic() { game_object = nullptr; }

	void update()
	{
		game_object->transform.position.set(game_object->transform.position.x + (speed.x * delta_t), game_object->transform.position.y + (speed.y * delta_t));

		if (game_object->transform.position.x > coin->transform.position.x - 10 && game_object->transform.position.x < coin->transform.position.x + 10)
		{
			coin->active = false;
			score = 10;
		}
	}

};
/*end ugly game logic*/


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

	Time::init();


	// EVERYTHING UNDERNEATH IS NOT ENGINE RELATED CODE AND CAN, AS WELL AS SHOULD, BE IGNORED
	// New ugly quick crunch code for TINR homeworks here (clean it up soon)
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Load sprites
	Sprite ground_sprite("level.bmp", renderer);
	Sprite robo_sprite("robo.bmp", renderer);
	Sprite end_text_sprite("text.bmp", renderer);
	Sprite bridge_sprite("bridge.bmp", renderer);
	Sprite button_sprite("switch.bmp", renderer);
	Sprite block_sprite("sprite.bmp", renderer);
	Sprite static_score_text("ScoreText.bmp", renderer);
	Sprite static_timer_text("ScoreText.bmp", renderer);
	Sprite t0("cifre.bmp", renderer);
	Sprite t1("cifre.bmp", renderer);
	Sprite t2("cifre.bmp", renderer);
	Sprite t3("cifre.bmp", renderer);
	Sprite t4("cifre.bmp", renderer);
	Sprite coin_sprite("coin.bmp", renderer);
	Sprite pause_sprite("pausebutton.bmp", renderer);
	Sprite overlay_sprite("black.bmp", renderer);

	static_score_text.origin.w = 110;
	static_timer_text.origin.x = 112;
	static_timer_text.origin.w = 76;
	t0.origin.w = 32;
	t1.origin.w = 32;
	t2.origin.w = 32;
	t3.origin.w = 32;
	t4.origin.w = 32;

	GameObject camera;
	camera.active = true;


	// Prepare and load world
	vector<GameObject*> world;
	
	// Init game objects
	GameObject ground("ground");
	GameObject robo("robo");
	GameObject end_text("end_text");
	GameObject bridge("bridge");
	GameObject button("button");
	GameObject block("block");
	GameObject score_text("score_text");
	GameObject timer_text("timer_text");
	GameObject timer0("timer 0");
	GameObject timer1("timer 1");
	GameObject timer2("timer 2");
	GameObject s0("score 0");
	GameObject s1("score 1");
	GameObject coin("coin");
	GameObject pause("pause button");
	GameObject overlay("black");
	
	// Add game objects on the world
	//world.push_back(&camera);
	world.push_back(&ground);
	world.push_back(&robo);
	world.push_back(&score_text);
	world.push_back(&timer_text);
	world.push_back(&timer0);
	world.push_back(&timer1);
	world.push_back(&timer2);
	world.push_back(&s0);
	world.push_back(&s1);
	world.push_back(&coin);
	world.push_back(&pause);
	//world.push_back(&end_text);
	//world.push_back(&bridge);
	//world.push_back(&button);
	//world.push_back(&block);

	// Add render components to game objects
	ground.components.push_back(new Renderer(&ground, renderer, &camera, &ground_sprite));
	robo.components.push_back(new Renderer(&robo, renderer, &camera, &robo_sprite));
	end_text.components.push_back(new Renderer(&end_text, renderer, &camera, &end_text_sprite));
	bridge.components.push_back(new Renderer(&bridge, renderer, &camera, &bridge_sprite));
	button.components.push_back(new Renderer(&button, renderer, &camera, &button_sprite));
	block.components.push_back(new Renderer(&block, renderer, &camera, &block_sprite));
	score_text.components.push_back(new Renderer(&score_text, renderer, &camera, &static_score_text, true));
	timer_text.components.push_back(new Renderer(&timer_text, renderer, &camera, &static_timer_text, true));
	timer0.components.push_back(new Renderer(&timer0, renderer, &camera, &t0, true));
	timer1.components.push_back(new Renderer(&timer1, renderer, &camera, &t1, true));
	timer2.components.push_back(new Renderer(&timer2, renderer, &camera, &t2, true));
	s0.components.push_back(new Renderer(&s0, renderer, &camera, &t3, true));
	s1.components.push_back(new Renderer(&s1, renderer, &camera, &t4, true));
	coin.components.push_back(new Renderer(&coin, renderer, &camera, &coin_sprite));
	pause.components.push_back(new Renderer(&pause, renderer, &camera, &pause_sprite, true));
	overlay.components.push_back(new Renderer(&overlay, renderer, &camera, &overlay_sprite, true));
	/*
	score_text.transform.setScale(0.75);
	timer_text.transform.setScale(0.75);
	timer0.transform.setScale(0.75);
	timer1.transform.setScale(0.75);
	timer2.transform.setScale(0.75);*/

	s0.transform.position.y = 31;
	s1.transform.position.y = 31;
	timer0.transform.position.y = 31;
	timer1.transform.position.y = 31;
	timer2.transform.position.y = 31;

	s1.active = false;
	s1.transform.position.x = 21;
	timer0.transform.position.x = 500;
	timer1.transform.position.x = 21 + 500;
	timer2.transform.position.x = 42 + 500;

	timer_text.transform.position.x = 500;

	pause.transform.position.x = 250;

	SDL_Event event;
	bool done = false;

	Uint32 t_curr = 0;
	Uint32 t_last = SDL_GetTicks();
	Uint32 t_render = 0;
	Uint32 t_total = 0;
	
	Uint32 mouse_state;
	int mouse_x, mouse_y;
	bool not_released = false;

	//robo.active = true;
	float rotato_potato = 0;

	// RELEASE BUILD STRESS TEST - i7 4790K @ 4.4 GHz & GTX 970 
	//				 can render 175000 static objects at 60 fps
	//				 can render 50000 rotating objects at 30 fps
	/*
	int stress_number = 100;
	int sqr_sn = (int)sqrtf(stress_number);
	if (sqr_sn == 0)
		sqr_sn = 1;
	world.clear();		// Clear world for stress testing
	world.resize(stress_number);	// Resize in advance for stress testing, to reduce loading time
	for (int i = 0; i < stress_number; i++)
	{
		world[i] = new GameObject();																						// Create new game object
		world[i]->active = true;																							// Activate object in scene
		world[i]->components.push_back(new Renderer(world[i], renderer, &camera, &block_sprite));							// Add abstract renderer component
		world[i]->transform.position.set((i % sqr_sn) * (RENDER_MIN_X / sqr_sn), (i / sqr_sn) * (RENDER_MIN_Y / sqr_sn));	// Position object in grid
	}
	*/

	camera.transform.position.set(32, -150);
	//camera.transform.position.set(400-32, -300+32);
	ground.transform.position.set(-64, -112);
	robo.transform.position.set(-320, 0);

	RoboLogic robot(&robo);
	robo.components.push_back(&robot);
	robot.coin = &coin;
	
	coin.transform.position.set(-260, 0);
	float time = 105;
	float temp_time = 100;
	int out_timer = 0;
	bool on_hold = false;
	bool on_hold_1 = false;
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

	cout << "Seconds since startup: " << Time::timeSinceStartup() << endl;
	Time::recalculate();
	Time::recalculateFixed();
	Time::recalculate();
	Time::recalculateFixed();

	while (!done) 
	{
		// Calculate delta time
		t_curr = SDL_GetTicks();
		t_delta = (float)(t_curr - t_last) / 1000.0f;
		t_last = t_curr;

		// robo code handle here
		robot.delta_t = t_delta;

		if (robot.score == 10)
		{
			s1.active = true;
			t3.origin.x = 32;
		}

		temp_time = roundf(time);

		t0.origin.x = 32 * ((int)(temp_time / 100) % 10);
		t1.origin.x = 32 * ((int)(temp_time / 10) % 10);
		t2.origin.x = 32 * ((int)temp_time % 10);

		if(!on_hold)
			time -= t_delta;


		//rotato_potato += 90 * t_delta;
		//robo.transform.setScale(rotato_potato/50);		// scale robot
		//robo.transform.setRotation(rotato_potato);		// rotate robot
		//camera.transform.position.x += 60 * t_delta;	// move camera
		
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
					if (mouse_x > (pause.transform.position.x - pause_sprite.origin.w) && mouse_x < (pause.transform.position.x + pause_sprite.origin.w) && mouse_y < pause_sprite.origin.h)
					{
						if (on_hold)
						{
							on_hold = false;

						}
						else
						{
							on_hold = true;
							on_hold_1 = false;
						}
					}

					robot.speed.x = 60;
					not_released = true;
				}
			}
			if (event.type == SDL_QUIT)
			{
				done = true;
			}
			if (event.type == SDL_MOUSEBUTTONUP || event.type == SDL_FINGERUP)
			{
				robot.speed.x = 0;
				not_released = false;
			}
		}

		// Update all world objects
		if (!on_hold || !on_hold_1)
		{
			for (int i = 0; i < world.size(); i++)
			{
				world[i]->transform.setRotation(rotato_potato);
				world[i]->update();
			}
			camera.transform.position.x += robot.speed.x *t_delta;

			if (on_hold)
			{
				on_hold_1 = true;
				overlay.update();
			}

			// Display render
			SDL_RenderPresent(renderer);
		}

		// Calculate render time
		//t_render = SDL_GetTicks() - t_last;
		t_render = round(Time::diffInMs(Time::getLastTick(), Time::getCurrTick())*1000);


		// Print FPS every 15th frame
		if (out_timer == 15)
		{
			if (t_render > 0)
				cout << t_render << " " << 1000.0 / t_render << endl;
			else
				cout << "Infinite power!" << endl;
			out_timer = 0;
		}
		else
			out_timer++;
		
		// Let's give CPU some time to rest
		if (t_render < 8)		// Almost 120 fps
			SDL_Delay(8 - t_render); // Sleep until next frame
		else if (t_render < 11) // Almost 90 fps
			SDL_Delay(11 - t_render);
		else if (t_render < 16)	// Almost 60 fps
			SDL_Delay(16 - t_render);
		else if (t_render < 22) // Almost 45 fps
			SDL_Delay(22 - t_render);
		else if (t_render < 33) // Almost 30 fps
			SDL_Delay(33 - t_render);
			
		// Clear screen for new render
		if(!on_hold)
			SDL_RenderClear(renderer);

		// Recalculate delta_t
		Time::recalculate();
	}

	// todo - fix destructors
	// todo - fix core classes
	// todo - handle hierarchy
	// todo - create world container class and level class
	// todo - physics and collision

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