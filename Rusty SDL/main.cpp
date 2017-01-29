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
#include <SDL_mixer.h>
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
#include "RustyEngine/Audio.h"
#include "RustyEngine/SoundEffect.h"
#include "RustyEngine/Music.h"
#include "RustyEngine/AudioSource.h"
#include "RustyEngine/Rigidbody.h"
#include "RustyEngine/Collider.h"
#include "RustyEngine/ColliderAAB.h"
#include "RustyEngine/ColliderAAHP.h"
#include "RustyEngine/ColliderCircle.h"
#include "RustyEngine/ColliderHP.h"
#include "RustyEngine/ColliderRectangle.h"

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
					Time::delta_t = 0;
					Time::fixed_delta_t = 0;
				}
			}
		}
		else
			level_switch = false;
	}
};

class Button2 : public Component
{
public:
	int w, h;
	GameObject* target_obj;
	bool fliped;
	Button2 *tar;

	Button2(int width, int height, GameObject* target, Button2* tar) { target_obj = target; w = width; h = height; fliped = false, this->tar = tar; }
	~Button2() { target_obj = nullptr; }

	void update()
	{
		if (fliped)
		{
			SDL_Delay(150);
			fliped = false;
			return;
		}
		if ((Input::getTouch() > 0 || Input::getMouseDown(Mousebutton::left) > 0))
		{
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
				if (target_obj == nullptr)
					exit(0);
				else
				{
					target_obj->active = true;
					this->game_object->active = false;
					tar->fliped = true;
					Time::delta_t = 0;
					Time::fixed_delta_t = 0;
				}
			}
		}
	}
};

class Button3 : public Component
{
public:
	int w, h;
	GameObject* target_obj;

	Button3(int width, int height, GameObject* target) { target_obj = target; w = width; h = height; }
	~Button3() { target_obj = nullptr; }

	void update()
	{
		if ((Input::getTouch() > 0 || Input::getMouseDown(Mousebutton::left) > 0))
		{
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
				if (target_obj == nullptr)
					exit(0);
				else
				{
					target_obj->active = false;
					this->game_object->active = false;
					Time::delta_t = 0;
					Time::fixed_delta_t = 0;
					SDL_Delay(150);
				}
			}
		}
	}
};

class Tester : public Component
{
	Rigidbody *rbody;
public:
	void fixedUpdate()
	{
		//rbody->a
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
	SoundEffect coin_fx;
	SoundEffect roll_fx;
	SoundEffect jump_fx;
	AudioSource *a_src;
	AudioSource *ac_src;
	Mix_Music *music = NULL;
	bool isPlaying = false;
	bool isJumping = false;
	bool inAir = false;
	float ground_y;

	RoboLogic() 
	{ 
		coin = nullptr;
		coin_fx.load("coin_fx.wav");
		roll_fx.load("engine_fx_loop.wav");
		jump_fx.load("jump_fx.wav");

		a_src = new AudioSource(&roll_fx);
		a_src->isPositional = false;
		a_src->play(-1);
		a_src->pause();

		//game_object->addComponent(a_src);

		ac_src = new AudioSource(&coin_fx);
		//ac_src->play(-1);

		//music = Mix_LoadMUS("c-sand.xm");
		if (music == NULL)
			cout << "Failed to load music" << Mix_GetError() << endl;
		else
		{
			Mix_PlayMusic(music, -1);
			//music.load("c-sand.xm");
			Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
		}
	}
	~RoboLogic() { game_object = nullptr; }

	void start()
	{
		coin->addComponent(ac_src);
	}

	void update()
	{
		return;
		//cout << Input::getMouseDown(Mousebutton::left) << " " << Input::getMouseDown(Mousebutton::middle) << " " << Input::getMouseDown(Mousebutton::right) << endl;
		if (Input::getTouch() > 0)
			speed.x = 60 * Input::getTouch() * Input::getMouseDown(Mousebutton::left);
		else
			speed.x = 60 * Input::getMouseDown(Mousebutton::left);

		if (Input::getMouseDown(Mousebutton::middle) && !isJumping && !inAir)
			isJumping = true;

		if (isJumping && !inAir)
		{
			speed.y = 60;
			isJumping = false;
			inAir = true;
			ground_y = game_object->transform.position.y;

			//jump_fx.play();
		}

		if(inAir)
			speed.y -= 150 * delta_t;

		if (speed.x > 0.01 && !isPlaying)
		{
			//a_src->resume();
			isPlaying = true;
		}
		
		if(speed.x < 0.01)
		{
			a_src->pause();
			isPlaying = false;
		}

		game_object->transform.position.set(game_object->transform.position.x + (speed.x * delta_t), game_object->transform.position.y + (speed.y * delta_t));

		if (game_object->transform.position.y < ground_y)
		{
			game_object->transform.position.y = ground_y;
			speed.y = 0;
			inAir = false;
		}

		// if rolled over coin
		if (game_object->transform.position.x > coin->transform.position.x - 10 && game_object->transform.position.x < coin->transform.position.x + 10 && coin->active)
		{
			//coin->active = false;
			//coin_fx.play();
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

	GameObject *m_on = new GameObject("music_on");
	GameObject *m_off = new GameObject("music_off");
	GameObject *s_on = new GameObject("effect_on");
	GameObject *s_off = new GameObject("effects_off");
	
	options->addObject(m_on);
	options->addObject(m_off);
	options->addObject(s_on);
	options->addObject(s_off);
	
	m_on->addComponent(new Renderer(new Sprite("button_music_on.bmp"), true));
	m_off->addComponent(new Renderer(new Sprite("button_music_off.bmp"), true));
	s_on->addComponent(new Renderer(new Sprite("button_effects_on.bmp"), true));
	s_off->addComponent(new Renderer(new Sprite("button_effects_off.bmp"), true));
	
	Button2 *bt;
	Button2 *bt2 = nullptr;
	bt = new Button2(120, 80, m_off, bt2);
	bt2 = new Button2(120, 80, m_on, bt);
	bt->tar = bt2;
	m_on->addComponent(bt);
	m_off->addComponent(bt2);

	bt = new Button2(120, 80, s_off, bt2);
	bt2 = new Button2(120, 80, s_on, bt);
	bt->tar = bt2;

	s_on->addComponent(bt);
	s_off->addComponent(bt2);
	
	m_on->transform.position.set(240, 0);
	m_off->transform.position.set(240, 0);
	s_off->transform.position.set(240, 80);
	s_on->transform.position.set(240, 80);
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

	
	
	
	GameObject *fake = new GameObject("fake score names");
	GameObject *fake2 = new GameObject("fake score numbers");
	GameObject *clear = new GameObject("clear highscore");
	score->addObject(fake);
	score->addObject(fake2);
	score->addObject(clear);
	fake->addComponent(new Renderer(new Sprite("score_fake.bmp"), true));
	fake2->addComponent(new Renderer(new Sprite("score_fake2.bmp"), true));
	clear->addComponent(new Renderer(new Sprite("clear.bmp"), true));
	clear->addComponent(new Button3(120, 80, fake));
	clear->addComponent(new Button3(120, 80, fake2));
	fake->transform.position.set(240, 40);
	fake2->transform.position.set(280, 40);
	clear->transform.position.set(0, 240);
}


// Main function
int main(int argc, char**argv)
{
	// Prepare SDL and the rest of the engine for usage
	Game::init();

	// Prepare and init world
	Game::world.name = "New dawn";
	Game::world.init();

	// Create new level and add it to the world
	Level game;
	Game::world.levels.push_back(&game);
	Game::world.levels.back()->active = true;

	// EVERYTHING UNDERNEATH IS NOT ENGINE RELATED CODE AND CAN, AS WELL AS SHOULD, BE IGNORED
	// New ugly quick crunch code for TINR homeworks here (clean it up soon)
	SDL_SetRenderDrawColor(Game::world.main_renderer, 255, 255, 255, 255);
	SDL_RenderClear(Game::world.main_renderer);

	// Load sprites
	Sprite ground_sprite("level.bmp");
	Sprite robo_sprite("robo.bmp");
	Sprite end_text_sprite("text.bmp");
	Sprite bridge_sprite("bridge.bmp");
	Sprite button_sprite("switch.bmp");
	//Sprite block_sprite("sprite_circle.bmp");
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
	GameObject block2("block2");
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
	
	Game::world.audio_listener = &robo;

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
	Game::world.levels.back()->addObject(&block);
	Game::world.levels.back()->addObject(&block2);
	
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
	block2.addComponent(new Renderer(&block_sprite));
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


	Rigidbody test_rigid; // make rigidbody
	Rigidbody test_rigid2;
	test_rigid.use_gravity = false;
	test_rigid.mass = 1; // set parameters
	test_rigid.drag = 1;
	test_rigid.angular_drag = 0.2;
	test_rigid2.use_gravity = false;
	test_rigid2.mass = 1; // set parameters
	test_rigid2.drag = 0;
	test_rigid2.angular_drag = 0.2;

	ColliderRectangle rect_col; // make some colliders
	rect_col.setSize(1); // set size

	block.addComponent(&test_rigid); // add rigidbody to the game object
	block.transform.setScale(4);
	block2.addComponent(&test_rigid2);
	block2.transform.setScale(2);

	ColliderCircle c_col1, c_col2;
	ColliderHP hp_col1;
	ColliderRectangle r_col1;
	//c_col1.setRadius(1);
	//c_col2.setRadius(1);

	// now add colliders to rigidbody
	//test_rigid.addCollider(&rect_col);

	//test_rigid.addCollider(&c_col1);
	test_rigid.addCollider(&hp_col1);
	//test_rigid2.addCollider(&c_col2);
	test_rigid2.addCollider(&r_col1);

	block.transform.position.set(0, 0);
	block.transform.setRotation(30 * DEG_TO_RAD);
	block2.transform.position.set(8, 1.5);

	test_rigid2.velocity.set(-3, 0);



















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


	camera.transform.position.set(1, -4.6875);
	//camera.transform.position.set(400-32, -300+32);
	ground.transform.position.set(-2, -3.5);
	//robo.transform.position.set(-320, 0);
	robo.transform.position.set(1, 0);

	RoboLogic robot;
	robo.addComponent(&robot);
	robot.coin = &coin;
	robot.start();
	
	coin.transform.position.set(-8.125, 0);
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

	Time::delta_t = 0;
	Time::fixed_delta_t = 0;

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
		//camera.transform.position.x += robot.speed.x * Time::delta_t;	// move camera
		camera.transform.position = block.transform.position;
		
		// Game world testing
		if (game.active)
		{
			//test_rigid.addForceAtPosition(Vec2(0, 1), Vec2(block.transform.position.x + 1, block.transform.position.y));
			//test_rigid2.addForce(Vec2(-10, 0));
		}

		// Update all world objects
		Game::world.update();

		//cout << test_rigid.angular_velocity * RAD_TO_DEG << endl;

		// Display render
		Game::render();

		// Calculate render time
		t_render = round(Time::diffInMs(Time::getLastTick(), Time::getCurrTick())*1000);


		// Print FPS every 15th frame
		out_timer = 0;
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
		//Time::recalculateFixed();
		Time::fixed_delta_t = 0.01;
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