#include "Level.h"

namespace RustyEngine
{
	//	Constructors
	Level::Level()
	{
		loaded = false;
		active = false;
		name = "";
		filepath = "";
		objects.clear();
	}

	Level::Level(string filepath, string name)
	{
		loaded = false;
		active = false;
		this->name = name;
		this->filepath = filepath;
		objects.clear();

		load(filepath);
	}


	// Destructor
	Level::~Level()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			delete objects[i];
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

		return;
	}


	// Delete every object in this level (yes, all of them)
	void Level::removeAll()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			delete objects[i];
		}
		objects.clear();
	}


	// Add existing game object to the level
	void Level::addObject(GameObject * g_object)
	{
		objects.push_back(g_object);
	}


	// Create new game object and add it to the level. Returns pointer to create object.
	GameObject* Level::createObject(string name)
	{
		objects.push_back(new GameObject(name));
		return nullptr;
	}


	// Find first object in level with given name (case-sensitive) and return it
	GameObject* Level::getObjectByName(string name)
	{
		for (int i = 0; i < objects.size(); i++)
			if (objects[i]->name == name)
				return objects[i];

		return nullptr;
	}


	// Update every active game object in the level
	void Level::update()
	{
		for (int i = 0; i < objects.size(); i++)
			if(objects[i]->active)
				objects[i]->update();
	}
}