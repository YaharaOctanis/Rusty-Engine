#ifndef RUSTYENGINE_LEVEL_H
#define RUSTYENGINE_LEVEL_H

#pragma once

#include "GameObject.h"
#include <string>

namespace RustyEngine
{
	class Level
	{
	private:
		bool loading;
		bool loaded;
		bool active;
		string filepath;
		vector<GameObject*> objects;

	public:
		string name;

		//	Constructors
		Level();
		Level(string filepath, string name="");

		// Destructor
		~Level();

		// Level loaders - TODO actual loading part
		bool load(string filepath);							// Load level from given file and return true, if successful. Return false otherwise.
		void asyncLoad(string filepath, void(*)(Level*));	// Load level on a seperate thread, and call callback function when done.
															//  Example: my_level.asyncLoad("level_file.txt", myCallbackFunction);
															//  Method does not handle any exceptions triggered by thread creation.

		// Object manipulation
		void removeAll();						// Delete every object in this level (yes, all of them)
		void addObject(GameObject *g_object);	// Add existing game object to the level
		GameObject* createObject(string name="game_object");				// Create new game object and add it to the level. Returns pointer to create object.

		// Search
		GameObject* getObjectByName(string name);

		// Interaction
		void update();
	};
}

#endif // RUSTYENGINE_LEVEL_H