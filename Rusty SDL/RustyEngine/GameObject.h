#ifndef RUSTYENGINE_GAMEOBJECT_H
#define RUSTYENGINE_GAMEOBJECT_H

#pragma once

#include "Transform.h"
#include "Component.h"
#include "RustyConstants.h"
#include <vector>

namespace RustyEngine
{
	using namespace std;

	//  Represents every object on the game scene. Base game object container.
	class GameObject
	{
	private:
		vector<Component*> components;	// Array of components added to the game object (pointer array for faster resizing)

	public:
		bool active;					// Do not update or render object if not active
		string name;					// Name for reference
		string tag;						// Tag for reference
		Transform transform;			// Position, rotation and scale of the object (must have for every gameobject)

		GameObject();
		GameObject(string name);
		~GameObject();

		void update();								// Update function called once per render update (render loop)
		void guiUpdate();							// TODO rethink this function
		void addComponent(Component* component);	// Add component to the game object
	};
}

#endif // RUSTYENGINE_GAMEOBJECT_H