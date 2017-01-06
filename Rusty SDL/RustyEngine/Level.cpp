#include "Level.h"

namespace RustyEngine
{
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


	Level::~Level()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			delete objects[i];
		}
	}

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

	void Level::asyncLoad(string filepath, void(*)(Level *))
	{
		active = false;
		loaded = false;
		loading = true;
		this->filepath = filepath;

		// Create thread and call load(filepath) on that thread

		return;
	}

	void Level::removeAll()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			delete objects[i];
		}
		objects.clear();
	}

	void Level::addObject(GameObject * g_object)
	{
		objects.push_back(g_object);
	}

	GameObject* Level::createObject(string name)
	{
		objects.push_back(new GameObject(name));
		return nullptr;
	}

	GameObject* Level::getObjectByName(string name)
	{
		for (int i = 0; i < objects.size(); i++)
			if (objects[i]->name == name)
				return objects[i];

		return nullptr;
	}

	void Level::update()
	{
		for (int i = 0; i < objects.size(); i++)
			if(objects[i]->active)
				objects[i]->update();
	}
}