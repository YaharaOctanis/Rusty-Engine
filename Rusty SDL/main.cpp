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
#include "RustyEngine/Input.h"
#include "RustyEngine/Level.h"
#include "RustyEngine/World.h"
#include "RustyEngine/Game.h"

// TODO - create renderer loop class/thingy

using namespace std;
using namespace RustyEngine;
//using namespace concurrency;  // For use with amp

bool level_switch = false;

class Button : public Component
{
public:
	int w, h;
	Level* parent_level;
	Level* target_level;

	Button(int width, int height, Level* t, Level* p) { target_level = t; parent_level = p; w = width; h = height; }
	~Button() { target_level = nullptr; }

	void update()
	{
		if ((Input::getTouch() > 0 || Input::getMouseDown(Mousebutton::left) > 0))
		{
			if (level_switch)
				return;
			// If button is pressed
			if ((Input::getTouchPos().x > game_object->transform.position.x &&
				Input::getTouchPos().y > game_object->transform.position.y &&
				Input::getTouchPos().x < game_object->transform.position.x + w &&
				Input::getTouchPos().y < game_object->transform.position.y + h) ||
				(Input::getMousePos().x > game_object->transform.position.x &&
					Input::getMousePos().y > game_object->transform.position.y &&
					Input::getMousePos().x < game_object->transform.position.x + w &&
					Input::getMousePos().y < game_object->transform.position.y + h))
			{
				if (target_level == nullptr)
					exit(0);
				else
				{
					target_level->active = true;
					parent_level->active = false;
					level_switch = true;
				}
			}
		}
		else
			level_switch = false;
	}
};

/*Ugly game logic here*/
class RoboLogic : public Component
{
public:

	GameObject* coin;
	float score = 0;
	Vec2 speed;
	float delta_t;
	RoboLogic() { coin = nullptr; }
	~RoboLogic() { game_object = nullptr; }

	void update()
	{
		if (Input::getTouch() > 0)
			speed.x = 60 * Input::getTouch();
		else
			speed.x = 60 * Input::getMouseDown(Mousebutton::left);

		game_object->transform.position.set(game_object->transform.position.x + (speed.x * delta_t), game_object->transform.position.y + (speed.y * delta_t));

		if (game_object->transform.position.x > coin->transform.position.x - 10 && game_object->transform.position.x < coin->transform.position.x + 10)
		{
			coin->active = false;
			score = 10;
		}
	}

};
/*end ugly game logic*/

void main_menu_load(Level *menu, Level *game, Level *options, Level *score)
{
	GameObject *temp;

	temp = new GameObject("new game");
	temp->addComponent(new Renderer(new Sprite("button_newgame.bmp"), true));
	temp->addComponent(new Button(120, 80, game, menu));
	temp->transform.position.set(0, 0);
	menu->addObject(temp);

	temp = new GameObject("options");
	temp->addComponent(new Renderer(new Sprite("button_options.bmp"), true));
	temp->addComponent(new Button(120, 80, options, menu));
	temp->transform.position.set(0, 80);
	menu->addObject(temp);

	temp = new GameObject("score");
	temp->addComponent(new Renderer(new Sprite("button_score.bmp"), true));
	temp->addComponent(new Button(120, 80, score, menu));
	temp->transform.position.set(0, 160);
	menu->addObject(temp);

	temp = new GameObject("exit");
	temp->addComponent(new Renderer(new Sprite("button_exit.bmp"), true));
	temp->addComponent(new Button(120, 80, nullptr, menu));
	temp->transform.position.set(0, 240);
	menu->addObject(temp);
}

void options_menu_load(Level *options, Level *menu)
{
	GameObject *temp;

	temp = new GameObject("back");
	temp->addComponent(new Renderer(new Sprite("button_back.bmp"), true));
	temp->addComponent(new Button(120, 80, menu, options));
	temp->transform.position.set(0, 0);
	options->addObject(temp);

	temp = new GameObject("options_text");
	temp->addComponent(new Renderer(new Sprite("options_text.bmp"), true));
	temp->transform.position.set(140, 0);
	options->addObject(temp);
}

void score_menu_load(Level *score, Level *menu)
{
	GameObject *temp;

	temp = new GameObject("back");
	temp->addComponent(new Renderer(new Sprite("button_back.bmp"), true));
	temp->addComponent(new Button(120, 80, menu, score));
	temp->transform.position.set(0, 0);
	score->addObject(temp);

	temp = new GameObject("score_text");
	temp->addComponent(new Renderer(new Sprite("score_text.bmp"), true));
	temp->transform.position.set(140, 0);
	score->addObject(temp);
}


// Main function
int main(int argc, char**argv)
{
	// Prepare SDL and the rest of the engine for usage
	Game::init();

	// Prepare and init world
	Game::world.name = "new dawn";
	Game::world.init();

	// Create new level and add it to the world
	Level game;
	Game::world.levels.push_back(&game);
	Game::world.levels.back()->active = true;

	// EVERYTHING UNDERNEATH IS NOT ENGINE RELATED CODE AND CAN, AS WELL AS SHOULD, BE IGNORED
	// New ugly quick crunch code for TINR homeworks here (clean it up soon)
	SDL_SetRenderDrawColor(Game::world.main_renderer, 0, 0, 0, 255);
	SDL_RenderClear(Game::world.main_renderer);

	// Load sprites
	Sprite ground_sprite("level.bmp");
	Sprite robo_sprite("robo.bmp");
	Sprite end_text_sprite("text.bmp");
	Sprite bridge_sprite("bridge.bmp");
	Sprite button_sprite("switch.bmp");
	Sprite block_sprite("sprite.bmp");
	Sprite static_score_text("ScoreText.bmp");
	Sprite static_timer_text("ScoreText.bmp");
	Sprite t0("cifre.bmp");
	Sprite t1("cifre.bmp");
	Sprite t2("cifre.bmp");
	Sprite t3("cifre.bmp");
	Sprite t4("cifre.bmp");
	Sprite coin_sprite("coin.bmp");
	Sprite pause_sprite("pausebutton.bmp");
	Sprite overlay_sprite("black.bmp");

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
	Game::world.active_camera = &camera;
	
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

	// Prepare and load level
	// Add game objects on the level
	Game::world.levels.back()->addObject(&ground);
	Game::world.levels.back()->addObject(&robo);
	Game::world.levels.back()->addObject(&score_text);
	Game::world.levels.back()->addObject(&timer_text);
	Game::world.levels.back()->addObject(&timer0);
	Game::world.levels.back()->addObject(&timer1);
	Game::world.levels.back()->addObject(&timer2);
	Game::world.levels.back()->addObject(&s0);
	Game::world.levels.back()->addObject(&s1);
	Game::world.levels.back()->addObject(&coin);
	Game::world.levels.back()->addObject(&pause);
	//world.push_back(&end_text);
	//world.push_back(&bridge);
	//world.push_back(&button);
	//world.push_back(&block);

	// Add render components to game objects
	ground.addComponent(new Renderer(&ground_sprite));
	robo.addComponent(new Renderer(&robo_sprite));
	end_text.addComponent(new Renderer(&end_text_sprite));
	bridge.addComponent(new Renderer(&bridge_sprite));
	button.addComponent(new Renderer(&button_sprite));
	block.addComponent(new Renderer(&block_sprite));
	score_text.addComponent(new Renderer(&static_score_text, true));
	timer_text.addComponent(new Renderer(&static_timer_text, true));
	timer0.addComponent(new Renderer(&t0, true));
	timer1.addComponent(new Renderer(&t1, true));
	timer2.addComponent(new Renderer(&t2, true));
	s0.addComponent(new Renderer(&t3, true));
	s1.addComponent(new Renderer(&t4, true));
	coin.addComponent(new Renderer(&coin_sprite));
	pause.addComponent(new Renderer(&pause_sprite, true));
	overlay.addComponent(new Renderer(&overlay_sprite, true));
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

	bool done = false;

	//Uint32 t_curr = 0;
	//Uint32 t_last = SDL_GetTicks();
	Uint32 t_render = 0;
	//Uint32 t_total = 0;
	
	//Uint32 mouse_state;
	//int mouse_x, mouse_y;
	//bool not_released = false;

	//robo.active = true;
	//float rotato_potato = 0;


	camera.transform.position.set(32, -150);
	//camera.transform.position.set(400-32, -300+32);
	ground.transform.position.set(-64, -112);
	robo.transform.position.set(-320, 0);

	RoboLogic robot;
	robo.addComponent(&robot);
	robot.coin = &coin;
	
	coin.transform.position.set(-260, 0);
	float time = 105;
	float temp_time = 100;
	int out_timer = 0;
	bool on_hold = false;
	bool on_hold_1 = false;
	SDL_SetRenderDrawBlendMode(Game::world.main_renderer, SDL_BLENDMODE_ADD);

	Level menu, options, score;
	game.active = false;
	menu.active = true;
	options.active = false;
	score.active = false;

	Game::world.levels.push_back(&menu);
	Game::world.levels.push_back(&options);
	Game::world.levels.push_back(&score);

	main_menu_load(&menu, &game, &options, &score);
	options_menu_load(&options, &menu);
	score_menu_load(&score, &menu);

	pause.addComponent(new Button(128, 32, &menu, &game));

	cout << "Seconds since startup: " << Time::timeSinceStartup() << endl;
	Time::recalculate();
	Time::recalculateFixed();
	Time::recalculate();
	Time::recalculateFixed();

	while (!done) 
	{
		Input::update();
		// robo code handle here
		robot.delta_t = Time::delta_t;

		if (robot.score == 10)
		{
			s1.active = true;
			t3.origin.x = 32;
		}

		temp_time = roundf(time);

		t0.origin.x = 32 * ((int)(temp_time / 100) % 10);
		t1.origin.x = 32 * ((int)(temp_time / 10) % 10);
		t2.origin.x = 32 * ((int)temp_time % 10);

		time -= Time::delta_t;


		//rotato_potato += 90 * t_delta;
		//robo.transform.setScale(rotato_potato/50);		// scale robot
		//robo.transform.setRotation(rotato_potato);		// rotate robot
		camera.transform.position.x += robot.speed.x * Time::delta_t;	// move camera
		
		// Update all world objects
		Game::world.update();

		// Display render
		Game::render();

		// Calculate render time
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
		SDL_RenderClear(Game::world.main_renderer);

		// Recalculate delta_t
		Time::recalculate();
	}

	// todo - fix destructors
	// todo - fix core classes
	// todo - handle hierarchy
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






// OLD RELEASE BUILD STRESS TEST - i7 4790K @ 4.4 GHz & GTX 970 
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