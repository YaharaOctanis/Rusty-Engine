#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#pragma once

#include "Transform.h"
#include "Component.h"
#include "Rusty_Constants.h"
#include <vector>

using namespace std;

//  Represents every object on the game scene. Base game object container.
class GameObject
{
public:
	bool active;					// Do not update or render object if not active
	string name;					// Name for reference
	Transform transform;			// Position, rotation and scale of the object (must have for every gameobject)
	vector<Component*> components;	// Array of components added to the game object (pointer array for faster resizing)

	GameObject();
	GameObject(string name);
	~GameObject();

	void update();					// Update function called once per render update (render loop)
};

#endif // GAMEOBJECT_H