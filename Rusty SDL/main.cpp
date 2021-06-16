#if defined _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS		// SDL's C code will trigger warnings otherwise
	#define _RUSTY_WINDOWS_BUILD
	#if _MSC_VER<1800
		#error Must compile with MSVC++ version 18 or newer (vs2013)
	#endif
#else
	#error Must compile with MSVC++ version 18 or newer (vs2013)
#endif


#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_hints.h>
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
#include "RustyEngine/Physics.h"
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


// Main function
int main(int argc, char**argv)
{
	// Prepare SDL and the rest of the engine for usage
	Game::init();

	// Prepare and init world
	Game::world.name = "Rusty editor";
	Game::world.init();

	// Create new level and load it from a file
	Level test_level;
	Game::world.levels.push_back(&test_level);
	Game::world.levels.back()->active = true;

	// EVERYTHING UNDERNEATH IS NOT ENGINE RELATED CODE AND IS ONLY USED FOR TESTING
	SDL_SetRenderDrawColor(Game::world.main_renderer, 255, 255, 255, 255);
	SDL_RenderClear(Game::world.main_renderer);

	// Load sprite
	Sprite test_sprite("sprite.bmp");

	// Init game objects
	GameObject block1("block 1");
	GameObject block2("block 2");
	GameObject camera("camera");

	// Add render components to game objects
	block1.addComponent(new Renderer(&test_sprite));
	block2.addComponent(new Renderer(&test_sprite));

	block2.transform.setScale(2.0f);

	// Add game objects to the level
	test_level.addObject(&block1);
	test_level.addObject(&block2);
	Game::world.active_camera = &camera;

	block1.transform.position.x = -1;
	block2.transform.position.x = 1;

	Uint32 t_render = 0;
	
	SDL_SetRenderDrawBlendMode(Game::world.main_renderer, SDL_BLENDMODE_ADD);
	test_level.active = true;

	//cout << "Seconds since startup: " << Time::timeSinceStartup() << endl; // fix this, is borked
	Time::recalculate();
	Time::recalculateFixed();
	Time::recalculate();
	Time::recalculateFixed();

	Time::delta_t = 0;
	Time::fixed_delta_t = 0;

	SDL_SetRenderDrawColor(Game::world.main_renderer, 32, 32, 32, 255);

	bool done = false;
	//Input::initRawInput();

	bool using_vsync = false;

	// Enable v-sync
	//SDL_SetHint(SDL_HINT_RENDER_VSYNC);
	//using_vsync = true;
	
	// Select target framerate - game will try to run a bit faster than target framerate
	float target_framerate = 60.0f;
	float target_frametime = 1000.0f / target_framerate;
	Uint32 t_delay = 0;

	while (!done) 
	{
		// Read and update user inputs
		Input::update();
		
		// Update all world objects
		Game::world.update();

		// Display render
		Game::render();

		// Calculate render time
		t_render = ceil(Time::diffInMs(Time::getLastTick(), Time::getCurrTick())*1000);

		// Calculate wait time for target framerate
		t_delay = target_frametime - t_render;

		// Let's give CPU some time to rest if possible (do not use delay if using v-sync)
		if (t_delay > 0 && !using_vsync)
			SDL_Delay(t_delay);

		// Clear screen for new render (TODO move this control to camera)
		SDL_RenderClear(Game::world.main_renderer);

		// Recalculate delta_t
		Time::recalculate();
		Time::recalculateFixed(); // this will become important once physics gets moved to a seperate thread
		//Time::fixed_delta_t = 0.01f; //for debugging physics
	}

	// todo - fix destructors
	// todo - fix core classes
	// todo - handle hierarchy
	// todo - physics and collision

	SDL_Log("End\n");
	return EXIT_SUCCESS;
}







///////////////////////////////////////////////////////////////////////////////
// START EXAMPLE CODE														 //
///////////////////////////////////////////////////////////////////////////////

bool level_switch = false;
Level* end_screen = nullptr;

bool option_sound_fx = true;
bool option_music = true;
int score = 0;
bool player_dead = false;
GameObject* player = nullptr;

void killPlayer()
{
	player_dead = true;
}

void string_trim(string& s, char c = ' ')
{
	// Trim front
	int trim_f_max = 0;
	for (trim_f_max = 0; s.at(trim_f_max) == c; trim_f_max++);
	if (trim_f_max > 0)
		s.erase(0, trim_f_max);

	// Trim back
	while (s.back() == c)
		s.pop_back();
}

void saveOptions()
{
	ofstream opsFile;
	opsFile.open("options.ini");
	if (!opsFile.is_open())
	{
		cout << "Error! Failed to save options, your settings will be lost when the game closes." << endl;
		return;
	}
	opsFile << "[Audio]" << endl;
	if (option_music)
		opsFile << "Music = true" << endl;
	else
		opsFile << "Music = false" << endl;

	if (option_sound_fx)
		opsFile << "SoundFX = true" << endl;
	else
		opsFile << "SoundFX = false" << endl;
	opsFile.close();
}

void loadOptions()
{
	ifstream opsFile;
	opsFile.open("options.ini");
	if (!opsFile.is_open())
	{
		cout << "Error! Failed to load options, keeping defaults." << endl;
		return;
	}

	string line;
	char* c_line;
	char* split_line;
	vector<string> split;

	while (getline(opsFile, line))
	{
		split.clear();
		// Convert string to C string and split it by delimiter
		c_line = Level::strdup(line.c_str());
		split_line = strtok(c_line, "=");

		// Then convert it to string vector list
		while (split_line != NULL)
		{
			split.push_back(string(split_line));
			split_line = strtok(NULL, "=");
		}

		if (split.size() <= 1)
			continue;

		string_trim(split[0], ' ');
		string_trim(split[1], ' ');

		if (split[0] == "Music" || split[0] == "music")
		{
			if (split[1] == "true" || split[1] == "TRUE")
				option_music = true;
			else if (split[1] == "false" || split[1] == "FALSE")
				option_music = false;
		}

		if (split[0] == "SoundFX" || split[0] == "soundfx")
		{
			if (split[1] == "true" || split[1] == "TRUE")
				option_sound_fx = true;
			else if (split[1] == "false" || split[1] == "FALSE")
				option_sound_fx = false;
		}
	}
	opsFile.close();
}

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
	Button2* tar;

	Button2() { target_obj = nullptr; w = 0; h = 0; fliped = false; tar = nullptr; }
	Button2(int width, int height, GameObject* target, Button2* tar) { target_obj = target; w = width; h = height; fliped = false, this->tar = tar; }
	~Button2() { target_obj = nullptr; }

	virtual void isPressed() { return; };
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
					isPressed();
					Time::delta_t = 0;
					Time::fixed_delta_t = 0;
				}
			}
		}
	}
};

class ButtonMusic : public Button2
{
public:
	ButtonMusic(int width, int height, GameObject* target, Button2* tar) { target_obj = target; w = width; h = height; fliped = false, this->tar = tar; }
	~ButtonMusic() {}

	void isPressed()
	{
		Music::toggleMusic();
		option_music = !option_music;
		saveOptions();
	}
};

class ButtonSoundFX : public Button2
{
public:
	ButtonSoundFX(int width, int height, GameObject* target, Button2* tar) { target_obj = target; w = width; h = height; fliped = false, this->tar = tar; }
	~ButtonSoundFX() {}

	void isPressed()
	{
		AudioSource::toggleAudio();
		option_sound_fx = !option_sound_fx;
		saveOptions();
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

/*Ugly game logic here*/
class RoboLogic : public Component
{
public:
	SoundEffect roll_fx;
	AudioSource a_source;
	Collider* wheel;
	float max_volume;

	Vec2 max_speed;
	Vec2 acc_force;
	Rigidbody* r_body;
	Renderer* rend;

	bool grounded;

	Music music;

	/*
	GameObject* coin;
	float score = 0;
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
	*/

	RoboLogic()
	{
		grounded = false;
		max_speed.set(4, 5.5);
		acc_force.set(250, 4000);
		max_volume = 15;

		//coin_fx.load("coin_fx.wav");
		//roll_fx.load("engine_fx_loop.wav");
		//jump_fx.load("jump_fx.wav");
	}
	~RoboLogic() { game_object = nullptr; }

	void start()
	{
		roll_fx.load("engine_fx_loop.wav");
		a_source.setSoundEffect(&roll_fx);
		a_source.play(-1);
		a_source.setVolume(0);
		game_object->tag = "player";
		r_body = game_object->getComponent<Rigidbody>();
		rend = game_object->getComponent<Renderer>();
		wheel = game_object->getComponent<ColliderCircle>();
		r_body->freeze_rotation = true;
		music.load("necromancers_castle.xm");
		music.play(-1);
		music.setVolume(10);
	}

	void onCollision(GameObject* g_obj, Vec2 col_normal)
	{
		if (!wheel->collided)
			return;

		Vec2 up(0, 1);
		if (g_obj->tag == "Untagged" || g_obj->tag == "")
		{
			float ang = wheel->last_col_normal.angleBetween(up);

			//cout << col_normal.x << " " << col_normal.y << endl;

			if (ang < 46 * DEG_TO_RAD && ang > -46 * DEG_TO_RAD)
				grounded = true;
		}
	}

	void update()
	{
		if (r_body == nullptr)
			return;

		if (r_body->velocity.x < 0)
			rend->flip_x = true;
		else if (r_body->velocity.x > 0)
			rend->flip_x = false;

		a_source.setVolume(roundf((fabsf(r_body->velocity.x) / fabsf(max_speed.x)) * max_volume));

		if (Input::getMouseDown(Mousebutton::left) <= 0 && Input::getTouch() <= 0)
		{
			if (grounded)
				r_body->drag = 10;
			else
				r_body->drag = 1;
			grounded = false;
			return;
		}
		r_body->drag = 1;

		Vec2 m_pos;

		if (Input::getTouch() > 0)
			m_pos.set(Input::getTouchPos());
		else
			m_pos.set(Input::getMousePos());

		m_pos = Game::world.screenAbsoluteToRelative(&m_pos);

		// Get mouse grid position in grid[5, 3]
		Vec2 grid_pos((m_pos.x + 1) / (2.0f / 5.0f), (m_pos.y + 1) / (2.0f / 3.0f));
		Vec2 acc(0, 0);

		//cout << grid_pos.x << " " << grid_pos.y << endl;

		// Move robot right if pressed on one of 2/5th of the screen
		if (grid_pos.x > 3 && r_body->velocity.x < max_speed.x) // if on right
			acc.x = acc_force.x;
		else if (grid_pos.x < 2 && r_body->velocity.x > -max_speed.x) // if on left
			acc.x = -acc_force.x;

		if (grid_pos.y > 2 && grounded && r_body->velocity.y < max_speed.y)
			acc.y = acc_force.y * max_speed.y;

		// Apply forces to the robot

		r_body->addForce(acc);

		// Clamp to max speed
		if (r_body->velocity.x > max_speed.x)
			r_body->velocity.x = max_speed.x;
		else if (r_body->velocity.x < -max_speed.x)
			r_body->velocity.x = -max_speed.x;

		if (r_body->velocity.y > max_speed.y)
			r_body->velocity.y = max_speed.y;
		grounded = false;
	}

};

class FireTrap : public Component
{
private:
	bool on;
	float on_timelimit;
	float off_timelimit;
	float timer;

	Renderer* rend;

public:
	FireTrap()
	{
		on = false;
		timer = 0.0f;
		on_timelimit = 1.0f;
		off_timelimit = 1.0f;
	}

	~FireTrap() {}

	void start()
	{
		on = false;
		timer = 0.0f;
		rend = game_object->getComponent<Renderer>();
	}

	void onCollision(GameObject* g_obj, Vec2 col_normal)
	{
		if (g_obj->tag == "player" && on)
			killPlayer();
	}

	void update()
	{
		timer += Time::delta_t;

		if (on && on_timelimit <= timer)
		{
			on = false;
			timer -= on_timelimit;
		}
		else if (!on && off_timelimit <= timer)
		{
			on = true;
			timer -= off_timelimit;
		}

		rend->active = on;
	}
};

class ScoreDisplay : public Component
{
public:
	Renderer* rend;
	int numbers;
	float render_offset;

	ScoreDisplay() {}
	~ScoreDisplay() {}

	void start()
	{
		numbers = 5;
		render_offset = 21;
		rend = game_object->getComponent<Renderer>();
		rend->active = false; // disable renderer so we can perform custom rendering
		rend->absolute = true; // position is in screen space
		rend->origin.w = 32;
		rend->origin.h = 32;
		rend->origin.y = 0;
		rend->sprite->cell_size.set(32, 32);
		rend->sprite->grid_size.set(10, 1);
		rend->grid_pos.set(0, 0);
	}

	void update()
	{
		if (rend == nullptr)
			return;

		Vec2 first_pos = game_object->transform.position;
		int c_score = score;
		int dnum = 0;
		rend->origin.x = 0;

		for (int i = numbers; i >= 0; i--)
		{
			if (i > 0)
				dnum = (int)(c_score / (powf(10, i))) % 10;
			else
				dnum = c_score % 10;

			rend->grid_pos.x = dnum;
			//rend->origin.x = dnum * 32; // choose number from numbers sprite
			rend->update(); // render
			game_object->transform.position.x += render_offset;
		}

		game_object->transform.position = first_pos;
	}
};

class Lava : public Component
{
public:
	Lava() {}
	~Lava() {}

	void onCollision(GameObject* g_obj, Vec2 col_normal)
	{
		if (g_obj->tag == "player")
			killPlayer();
	}
};

class Coin : public Component
{
public:
	SoundEffect coin_fx;
	AudioSource a_source;

	Coin()
	{
	}

	~Coin() {}

	void start()
	{
		game_object->active = true;
		coin_fx.load("coin_fx.wav");
		a_source.setSoundEffect(&coin_fx);
		a_source.isPositional = true;
		a_source.max_distance = 2.0f;
		a_source.setVolume(60);
	}

	void onCollision(GameObject* g_obj, Vec2 col_normal)
	{
		if (g_obj->tag == "player")
		{
			a_source.play();
			game_object->active = false;
			score += 100;
			//cout << score << endl;
		}
	}
};

class GameExit : public Component
{
public:
	Level* current;
	Level* next;

	GameExit() {}
	GameExit(Level* curr, Level* next) { current = curr; this->next = next; }
	~GameExit() {}

	void onCollision(GameObject* g_obj, Vec2 col_normal)
	{
		if (current == nullptr || next == nullptr)
			return;

		if (g_obj->tag == "player")
		{
			current->active = false;
			next->active = true;
			AudioSource::stopAll();
			Music::stopCurrent();
		}
	}

	void update()
	{
		if (player_dead)
		{
			if (current == nullptr || next == nullptr)
				return;

			current->active = false;
			next->active = true;
			AudioSource::stopAll();
			Music::stopCurrent();
		}
	}
};

class Mummy : public Component
{
public:
	Renderer* rend;
	Vec2 path_a, path_b; // 27, 31
	float vision_range;
	bool switch_dir;
	Vec2 target_pos;

	Vec2 speed;

	Mummy() {}
	Mummy(Vec2 a, Vec2 b) { path_a = a; path_b = b; }
	~Mummy() {}

	void start()
	{
		vision_range = 3.0f;
		speed.set(1, 1);
		switch_dir = false;
		target_pos = path_b;
		rend = game_object->getComponent<Renderer>();
		//rend = static_cast<Renderer*>(game_object->getComponent(typeid(Renderer)));
	}

	void update()
	{
		if (player == nullptr)
			return;

		Vec2 move(0, 0);
		target_pos.y = game_object->transform.position.y;

		// If player is in range
		if (game_object->transform.position.distanceTo(player->transform.position) <= vision_range)
		{
			// Move toward it
			if (player->transform.position.x - game_object->transform.position.x > 0)
				move.x = 1.5f;
			else
				move.x = -1.5f;
		}
		else
		{
			if (game_object->transform.position.distanceTo(target_pos) < 0.1)
			{
				if (switch_dir)
				{
					switch_dir = false;
					target_pos.x = path_b.x;
				}
				else
				{
					switch_dir = true;
					target_pos.x = path_a.x;
				}
			}

			if (target_pos.x - game_object->transform.position.x > 0)
				move.x = 1;
			else
				move.x = -1;
		}

		if (move.x < 0)
			rend->flip_x = false;
		else if (move.x > 0)
			rend->flip_x = true;

		game_object->transform.position.x += move.x * speed.x * Time::delta_t;
	}

};

class Camera : public Component
{
public:
	GameObject* target;
	float speed_x, speed_y;
	Vec2 actual_pos;
	//float pixel_size; // Pixel size in world space

	Camera() {}
	Camera(GameObject* t) { target = t; }
	~Camera() {}

	void start()
	{
		Game::world.active_camera = this->game_object;
		speed_x = 0.8f;
		speed_y = 0.04f;
		//Vec2 temp_vec(1,0), temp2_vec(0,0);
		//pixel_size = (Game::world.screenToWorldSpace(temp_vec) - Game::world.screenToWorldSpace(temp2_vec)).x;
		//pixel_size *= RENDER_SCALE;
	}

	void update()
	{
		Game::world.active_camera = this->game_object;

		if (target == nullptr)
			return;

		Vec2 a = game_object->transform.position;
		Vec2 b = game_object->transform.position;

		a.lerp(target->transform.position, speed_x);
		b.lerp(target->transform.position, speed_y);

		//a.x = a.x - (fmodf(a.x / pixel_size, 1) * pixel_size);
		//a.y = a.y - (fmodf(a.y / pixel_size, 1) * pixel_size);

		game_object->transform.position.set(a.x, b.y);
	}

};

class NameInput : Component
{
public:
	Level* target_level;
	string name;

	NameInput() { target_level = nullptr; name = ""; }
	~NameInput() { target_level = nullptr; }

	void update()
	{
		// do sdl input here, popup keyboard and all that good stuff
	}
};
/*end ugly game logic*/

void main_menu_load(Level* menu, Level* game, Level* options, Level* score)
{
	GameObject* temp;

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

	temp = new GameObject("camera");
	temp->active = true;
	temp->addComponent(new Camera());
	menu->addObject(temp);

	//temp = new GameObject("credits");
	//temp->addComponent(new Renderer(new Sprite("button_credits.bmp"), true));
	//temp->addComponent(new Button(120, 80, nullptr, menu));
	//temp->transform.position.set(0, 240);
	//menu->addObject(temp);
}

void options_menu_load(Level* options, Level* menu)
{
	GameObject* temp;

	temp = new GameObject("back");
	temp->addComponent(new Renderer(new Sprite("button_back.bmp"), true));
	temp->addComponent(new Button(120, 80, menu, options));
	temp->transform.position.set(0, 0);
	options->addObject(temp);

	temp = new GameObject("options_text");
	temp->addComponent(new Renderer(new Sprite("options_text.bmp"), true));
	temp->transform.position.set(140, 0);
	options->addObject(temp);

	GameObject* m_on = new GameObject("music_on");
	GameObject* m_off = new GameObject("music_off");
	GameObject* s_on = new GameObject("effect_on");
	GameObject* s_off = new GameObject("effects_off");

	options->addObject(m_on);
	options->addObject(m_off);
	options->addObject(s_on);
	options->addObject(s_off);

	m_on->addComponent(new Renderer(new Sprite("button_music_on.bmp"), true));
	m_off->addComponent(new Renderer(new Sprite("button_music_off.bmp"), true));
	s_on->addComponent(new Renderer(new Sprite("button_effects_on.bmp"), true));
	s_off->addComponent(new Renderer(new Sprite("button_effects_off.bmp"), true));

	ButtonMusic* btm, * btm2 = nullptr;
	btm = new ButtonMusic(120, 80, m_off, btm2);
	btm2 = new ButtonMusic(120, 80, m_on, btm);
	btm->tar = btm2;
	m_on->addComponent(btm);
	m_off->addComponent(btm2);

	ButtonSoundFX* bts, * bts2 = nullptr;
	bts = new ButtonSoundFX(120, 80, s_off, bts2);
	bts2 = new ButtonSoundFX(120, 80, s_on, bts);
	bts->tar = bts2;
	s_on->addComponent(bts);
	s_off->addComponent(bts2);

	m_on->transform.position.set(240, 0);
	m_off->transform.position.set(240, 0);
	s_off->transform.position.set(240, 80);
	s_on->transform.position.set(240, 80);

	loadOptions();

	if (option_music)
	{
		m_off->active = false;
		m_on->active = true;
		Music::enableMusic();
	}
	else
	{
		m_off->active = true;
		m_on->active = false;
		Music::disableMusic();
	}

	if (option_sound_fx)
	{
		s_off->active = false;
		s_on->active = true;
		AudioSource::enableAudio();
	}
	else
	{
		s_off->active = true;
		s_on->active = false;
		AudioSource::disableAudio();
	}

	GameObject* camera = new GameObject("camera");
	camera->active = true;
	camera->addComponent(new Camera());
	options->addObject(camera);
}

void score_menu_load(Level* score, Level* menu)
{
	GameObject* temp;

	temp = new GameObject("back");
	temp->addComponent(new Renderer(new Sprite("button_back.bmp"), true));
	temp->addComponent(new Button(120, 80, menu, score));
	temp->transform.position.set(0, 0);
	score->addObject(temp);

	temp = new GameObject("score_text");
	temp->addComponent(new Renderer(new Sprite("score_text.bmp"), true));
	temp->transform.position.set(140, 0);
	score->addObject(temp);




	GameObject* fake = new GameObject("fake score names");
	GameObject* fake2 = new GameObject("fake score numbers");
	GameObject* clear = new GameObject("clear highscore");
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

	GameObject* camera = new GameObject("camera");
	camera->active = true;
	camera->addComponent(new Camera());
	score->addObject(camera);
}

void credits_menu_load(Level* credits, Level* menu)
{
	GameObject* temp;

	temp = new GameObject("back");
	temp->addComponent(new Renderer(new Sprite("button_back.bmp"), true));
	temp->addComponent(new Button(120, 80, menu, credits));
	temp->transform.position.set(0, 0);
	credits->addObject(temp);

	//temp = new GameObject("credits_text");
	//temp->addComponent(new Renderer(new Sprite("credits.bmp"), true));
	//temp->transform.position.set(140, 0);

	GameObject* camera = new GameObject("camera");
	camera->active = true;
	camera->addComponent(new Camera());
	credits->addObject(camera);
}

void end_screen_load(Level* menu)
{
	if (end_screen != nullptr)
		return;

	end_screen = new Level();
	end_screen->name = "End score screen";
	end_screen->active = false;
}

void load_level(Level* lvl, Level* next)
{
	// Remove everything from the level first
	lvl->load(lvl->filepath);

	// Make and add camera
	GameObject* camera = new GameObject("camera");
	camera->active = true;
	camera->addComponent(new Camera(player));

	ColliderRectangle* rec = new ColliderRectangle();
	rec->setSize(1, 2);
	GameObject* mummy = new GameObject("mummy");
	Rigidbody* rb = new Rigidbody();
	mummy->active = true;
	mummy->transform.position.set(29, 1);
	mummy->addComponent(new Renderer(new Sprite("mummy.bmp")));
	mummy->addComponent(new Mummy(Vec2(27, 1), Vec2(31, 1)));
	mummy->addComponent(rb);
	rb->mass = 100;
	rb->use_gravity = true;
	rb->drag = 1;
	rb->freeze_rotation = true;
	rb->addCollider(rec);
	mummy->transform.setScale(0.75);

	GameObject* score_text = new GameObject("score");
	score = 0;
	score_text->addComponent(new Renderer(new Sprite("cifre.bmp")));
	score_text->addComponent(new ScoreDisplay());
	score_text->transform.position = Game::world.screenRelativeToAbsolute(&Vec2(-1, 0));
	score_text->transform.position.y = 20;
	score_text->transform.position.x += 20;

	lvl->addObject(player);	// Add player to middle (default) layer
	lvl->addObject(camera, 2);	// Add camera to foreground layer, so it gets updated last
	lvl->addObject(mummy);
	lvl->addObject(score_text);

	// Place robot to player start
	player->transform.position = lvl->getObjectByName("PlayerStart")->transform.position;

	// Finish level loading by adding non-engine components
	for (GameObject* go : lvl->getObjectsByTag("Fire trap"))
		go->addComponent(new FireTrap());
	//game_level1.getObjectByTag("Fire trap")->addComponent(new FireTrap());
	for (GameObject* go : lvl->getObjectsByTag("Coin"))
		go->addComponent(new Coin());
	for (GameObject* go : lvl->getObjectsByTag("Lava"))
		go->addComponent(new Lava());
	for (GameObject* go : lvl->getObjectsByTag("Exit"))
		go->addComponent(new GameExit(lvl, next));

	Game::world.audio_listener = player;
	// Levels loaded
}

void example_main()
{
	// Prepare SDL and the rest of the engine for usage
	Game::init();

	// Prepare and init world
	Game::world.name = "New dawn";
	Game::world.init();

	// Create new level and load it from a file
	Level game_level1;
	game_level1.filepath = "test_level.txt";
	Game::world.levels.push_back(&game_level1);
	Game::world.levels.back()->active = false;

	Level menu, options, score, credits;

	// EVERYTHING UNDERNEATH IS NOT ENGINE RELATED CODE AND CAN, AS WELL AS SHOULD, BE IGNORED
	// New ugly quick crunch code for TINR homeworks here (clean it up soon)
	SDL_SetRenderDrawColor(Game::world.main_renderer, 255, 255, 255, 255);
	SDL_RenderClear(Game::world.main_renderer);

	// Load sprites
	Sprite robot_sprite("robo.bmp");
	//Sprite number_sprite("cifre.bmp");
	Sprite pause_sprite("pausebutton.bmp");
	Sprite overlay_sprite("black.bmp");

	// Init game objects
	GameObject robot("robot");
	GameObject pause("pause button");
	GameObject overlay("black");

	player = &robot;

	// Prepare and load level
	// Add game objects on the level

	//game_level1.addObject(&robot);
	//game_level1.addObject(&pause);
	//game_level1.addObject(&overlay);

	// Add render components to game objects
	robot.addComponent(new Renderer(&robot_sprite));
	pause.addComponent(new Renderer(&pause_sprite, true));
	overlay.addComponent(new Renderer(&overlay_sprite, true));

	pause.transform.position = Game::world.screenRelativeToAbsolute(&Vec2(0, 0));
	pause.transform.position.x -= 64;
	pause.transform.position.y = 0;
	pause.addComponent(new Button(128, 32, &menu, &game_level1));

	robot.transform.setScale(0.5);

	Rigidbody robot_rigid; // make rigidbody for robot
	robot_rigid.use_gravity = true;
	robot_rigid.mass = 10; // set parameters
	robot_rigid.drag = 1;
	robot_rigid.angular_drag = 0.4f;
	RoboLogic robot_logic;
	robot.addComponent(&robot_logic);

	ColliderRectangle rect_col; // make collider for robot
	ColliderCircle circle_col;
	rect_col.setSize(0.51f, 1.9f); // set size
	circle_col.setRadius(0.5f);
	circle_col.offset.set(0.0f, -0.5f);

	robot.addComponent(&robot_rigid); // add rigidbody to the game object
	robot_rigid.addCollider(&circle_col);
	robot_rigid.addCollider(&rect_col); // add collider to the robot

	load_level(&game_level1, &menu);
	game_level1.addObject(&pause);

	Uint32 t_render = 0;

	SDL_SetRenderDrawBlendMode(Game::world.main_renderer, SDL_BLENDMODE_ADD);
	game_level1.active = false;
	menu.active = true;
	options.active = false;
	score.active = false;

	Game::world.levels.push_back(&menu);
	Game::world.levels.push_back(&options);
	Game::world.levels.push_back(&score);
	Game::world.levels.push_back(&credits);
	//Game::world.levels.push_back(end_screen);

	main_menu_load(&menu, &game_level1, &options, &score);
	options_menu_load(&options, &menu);
	score_menu_load(&score, &menu);
	credits_menu_load(&credits, &menu);

	//cout << "Seconds since startup: " << Time::timeSinceStartup() << endl; // fix this, is borked
	Time::recalculate();
	Time::recalculateFixed();
	Time::recalculate();
	Time::recalculateFixed();

	Time::delta_t = 0;
	Time::fixed_delta_t = 0;

	SDL_SetRenderDrawColor(Game::world.main_renderer, 32, 32, 32, 255);

	bool done = false;
	int out_timer = 0; // How many more frames do we have to render to display fps value
	//Input::initRawInput();

	while (!done)
	{
		// Read and update user inputs
		Input::update();

		// Update all world objects
		Game::world.update();

		// Display render
		Game::render();

		// Calculate render time
		t_render = round(Time::diffInMs(Time::getLastTick(), Time::getCurrTick()) * 1000);


		// Print FPS every 15th frame

		/*
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
		*/
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
		Time::recalculateFixed();
		//Time::fixed_delta_t = 0.01f;
	}

	// todo - fix destructors
	// todo - fix core classes
	// todo - handle hierarchy
	// todo - physics and collision

	SDL_Log("End\n");
	return;
}


///////////////////////////////////////////////////////////////////////////////
// END EXAMPLE CODE														 //
///////////////////////////////////////////////////////////////////////////////



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