#include "Level.h"
#include "Error.h"
#include "Renderer.h"
#include "ColliderRectangle.h"
#include "ColliderHP.h"
#include "Rigidbody.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "Game.h"
#include "Physics.h"

namespace RustyEngine
{
	char* Level::strdup(const char *src) {
		size_t len = strlen(src) + 1;
		char *s = (char*)malloc(len);
		if (s == NULL)
			return NULL;
		return (char *)memcpy(s, src, len);
	}


	//	Constructors
	Level::Level()
	{
		loaded = false;
		active = false;
		started = false;
		name = "";
		filepath = "";

		for(int i = 0; i < layer_count; i++)
			objects[i].clear();
	}

	Level::Level(string filepath, string name)
	{
		loaded = false;
		active = false;
		started = false;
		this->name = name;
		this->filepath = filepath;
		
		for (int i = 0; i < layer_count; i++)
			objects[i].clear();

		load(filepath);
	}


	// Destructor
	Level::~Level()
	{
		for (int n = 0; n < layer_count; n++)
		{
			for (unsigned int i = 0; i < objects[n].size(); i++)
			{
				delete objects[n][i];
			}
		}
	}


	// Load level from given file and return true, if successful. Return false otherwise.
	bool Level::load(string filepath)
	{
		active = false;
		loading = true;
		loaded = false;
		this->filepath = filepath;

		removeAll();

		// DO LEVEL LOAD HERE
		//	return false if level load fails

		/*
		load order

		load images to textures (sprites)
		create gameobjects and add them to the level
		add components to the gameobjects
			rigidbodies should be added before colliders
		
		*/

		// Structure
		/*

		Game object
		Game_object [name] [tag] [pos_x] [pos_y] [rotation] [scale_x] [scale_y]

		Renderer
		Renderer [filename] [grid_index] [layer]

		Box collider
		Box_collider [size_x] [size_y] [is_trigger]

		Line collider
		Line_collider [rotation]

		*/


		// first game object, then followed by it's components
		// type name tag layer pos_x pos_y rotation scale_x scale_y
		// [component_type component_vars]

		string line;
		char *c_line;
		char *split_line;
		vector<string> split;
		int line_number = 1;
		int layer = 1;
		GameObject *g_obj = nullptr;
		Renderer *rend = nullptr;
		Sprite *sprite = nullptr;
		ColliderRectangle *r_col = nullptr;
		ColliderHP *l_col = nullptr;

		std::ifstream file(filepath);

		// Open file
		if (file.is_open())
		{
			// Read each line
			while (std::getline(file, line))
			{
				// Convert string to C string and split it by delimiter
				c_line = Level::strdup(line.c_str());
				split_line = strtok(c_line, "|");
				
				// Then convert it to string vector list
				while (split_line != NULL)
				{
					split.push_back(string(split_line));
					split_line = strtok(NULL, "|");
				}

				if (split.size() <= 0)
					return false;

				// Parse line - LOADING PART

				// Handle game objects
				if (split[0] == "Game_object")
				{
					if (split.size() < 8)
					{
						std::cout << "LEVEL | Error parsing level. Game object is missing arguments. Line " << line_number << std::endl;
						return false;
					}

					if (g_obj != nullptr)
					{
						if (rend != nullptr)
							objects[layer].push_back(g_obj);
						else
							objects[1].push_back(g_obj);

						layer = 1;
					}
					g_obj = nullptr;

					g_obj = new GameObject();
					g_obj->name = split[1];
					g_obj->tag = split[2];
					g_obj->transform.position.set( stof(split[3]), stof(split[4]) );
					g_obj->transform.setRotation( -stof(split[5]) * DEG_TO_RAD );
					g_obj->transform.setScale( stof(split[6]), stof(split[7]) );
				}
				// Handle renderer component
				else if (split[0] == "Renderer")
				{
					rend = nullptr;
					sprite = nullptr;

					if (split.size() < 4)
					{
						std::cout << "LEVEL | Error parsing level. Renderer is missing arguments. Line " << line_number << std::endl;
						return false;
					}

					sprite = Game::world.getSpriteByFilename(split[1]);

					if (sprite == nullptr)
					{
						sprite = new Sprite(split[1], Game::world.main_renderer);
						Game::world.sprites.push_back(sprite);
					}

					rend = new Renderer(sprite);

					int grid_id = stoi(split[2]);
					int grid_cols = 1;
					int grid_x = 0, grid_y = 0;

					if (rend->origin.w % 32 == 0 && rend->origin.w > 64)
					{
						grid_cols = rend->origin.w / 32;
						grid_y = grid_id / grid_cols;
						grid_x = grid_id - (grid_cols * grid_y);
						rend->origin.w = 32;
						rend->origin.h = 32;
						sprite->cell_padding.set(0, 0);
						sprite->cell_size.set(32, 32);
						sprite->grid_size.set(grid_cols, grid_cols);
						sprite->grid_offset.set(0, 0);
						rend->grid_pos.set(grid_x, grid_y);
					}

					rend->origin.x = grid_x * 32;
					rend->origin.y = grid_y * 32;

					if (split[3] == "Default")
						layer = 1;
					else if (split[3] == "Background")
						layer = 0;
					else if (split[3] == "Foreground")
						layer = 2;
					else
						layer = 1;

					if(g_obj != nullptr)
						g_obj->addComponent(rend);
				}
				else if (split[0] == "Box_collider")
				{
					r_col = nullptr;

					if (split.size() < 4)
					{
						std::cout << "LEVEL | Error parsing level. Box collider is missing arguments. Line " << line_number << std::endl;
						return false;
					}

					r_col = new ColliderRectangle();
					r_col->setSize(stof(split[1]), stof(split[2]));

					if (split[3] == "False")
						r_col->isTrigger = false;
					else if (split[3] == "True")
						r_col->isTrigger = true;

					Physics::addCollider(r_col);

					if (g_obj != nullptr)
						g_obj->addComponent(r_col);
				}
				else if (split[0] == "Line_collider")
				{
					l_col = nullptr;
					l_col = new ColliderHP();

					Physics::addCollider(l_col);

					if (g_obj != nullptr)
						g_obj->addComponent(l_col);
				}
				else
				{
					std::cout << "LEVEL | Warning, unknown component or object. Skipping. Line " << line_number << std::endl;
				}				
				
				// Free memory used in conversion
				free(split_line);
				free(c_line);
				split.clear();
				line_number++;
			}
			file.close();
		}
		else
		{
			std::cout << "LEVEL | Failed to open file: " << filepath << std::endl;
			//exit(FILE_ACCESS_ERROR);
			return false;
		}

		// Just set them to null, we don't want to destory last objects added to the level :P
		g_obj = nullptr;
		rend = nullptr;
		sprite = nullptr;
		r_col = nullptr;
		l_col = nullptr;

		loaded = true;
		loading = false;
		return true;
	}


	// Load level on a seperate thread, and call callback function when done.
	//  Example: my_level.asyncLoad("level_file.txt", myCallbackFunction);
	//  Method does not handle any exceptions triggered by thread creation.
	//  Level is not thread protected, so do not access it until callback functions has done it's job 
	//   (you can use 'loaded' and 'active' flags to detect that, but only read from them, do not change them from main thread when loading asynchronously)
	void Level::asyncLoad(string filepath, void(*)(Level *))
	{
		active = false;
		loaded = false;
		loading = true;
		this->filepath = filepath;

		// Create thread and call load(filepath) on that thread
		//TODO thread call to load

		return;
	}


	// Delete every object in this level (yes, all of them)
	void Level::removeAll()
	{
		for (int n = 0; n < layer_count; n++)
		{
			for (unsigned int i = 0; i < objects[n].size(); i++)
			{
				delete objects[n][i];
			}
			objects[n].clear();
		}
	}


	// Add existing game object to the level
	void Level::addObject(GameObject * g_object, int layer)
	{
		objects[layer].push_back(g_object);
	}


	// Create new game object and add it to the level. Returns pointer to created object.
	GameObject* Level::createObject(string name, int layer)
	{
		objects[layer].push_back(new GameObject(name));
		return nullptr;
	}


	// Find first object in level with given name (case-sensitive) and return it
	GameObject* Level::getObjectByName(string name)
	{
		for(int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				if (objects[n][i]->name == name)
					return objects[n][i];

		return nullptr;
	}


	// Find first object in level with given tag (case-sensitive) and return it
	GameObject * Level::getObjectByTag(string tag)
	{
		for (int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				if (objects[n][i]->tag == tag)
					return objects[n][i];

		return nullptr;
	}


	// Find objects in level with given name (case-sensitive) and return them
	vector<GameObject*> Level::getObjectsByName(string name)
	{
		vector<GameObject*> objs;

		for (int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				if (objects[n][i]->name == name)
					objs.push_back(objects[n][i]);

		return objs;
	}


	// Find objects in level with given tag (case-sensitive) and return them
	vector<GameObject*> Level::getObjectsByTag(string tag)
	{
		vector<GameObject*> objs;

		for (int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				if (objects[n][i]->tag == tag)
					objs.push_back(objects[n][i]);

		return objs;
	}


	// Update every active game object in the level
	void Level::update()
	{
		if (!started) // auto call start if it was never started before
			start();

		for(int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				if(objects[n][i]->active)
					objects[n][i]->update();
	}


	// Start level (call start function on every component)
	void Level::start()
	{
		started = true;

		for (int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				objects[n][i]->start();
	}


	// Pause level (call pause function on every component)
	void Level::pause()
	{
		if (paused)
			return;

		paused = true;

		for (int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				objects[n][i]->pause();
	}


	// Resume level (call resume function on every component)
	void Level::resume()
	{
		if (!paused)
			return;

		paused = false;

		for (int n = 0; n < layer_count; n++)
			for (unsigned int i = 0; i < objects[n].size(); i++)
				objects[n][i]->resume();
	}
}