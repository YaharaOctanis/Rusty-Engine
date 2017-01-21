#ifndef RUSTYENGINE_LEVEL_H
#define RUSTYENGINE_LEVEL_H

#pragma once

#include "GameObject.h"
#include <string>

namespace RustyEngine
{
	// Represents game levels and holds information about every game object on that level
	// Level can be loaded from a file, by calling load or asyncLoad
	// Supports gameobject search
	// Can be disabled by setting 'active' to false.
	class Level
	{
	private:
		vector<GameObject*> objects;


	public:
		bool loading;			// Is level loading right now?
		bool loaded;			// Has level loaded?
		bool active;			// Is level active in game loop?
		string filepath;		// Reference to level file (dunno why store this.. maybe to reload the level?)
		string name;			// Level name (optional)

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
															//  Level is not thread protected, so do not access it until callback functions has done it's job 
															//   (you can use 'loaded' and 'active' flags to detect that, but only read from them, do not change them from main thread when loading asynchronously)

		// Object manipulation
		void removeAll();										// Delete every object in this level (yes, all of them)
		void addObject(GameObject *g_object);					// Add existing game object to the level
		GameObject* createObject(string name="game_object");	// Create new game object and add it to the level. Returns pointer to create object.

		// Search
		GameObject* getObjectByName(string name);			// Find first object in level with given name (case-sensitive) and return it

		// Interaction
		void update();										// Update every active game object in the level
	};
}

#endif // RUSTYENGINE_LEVEL_H