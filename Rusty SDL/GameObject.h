#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#pragma once

#include "Transform.h"
#include "Component.h"
#include <list>

using namespace std;

class GameObject
{
public:
	bool active;
	string name;
	Transform transform;
	list<Component*> components;

	GameObject();
	GameObject(string name);
	~GameObject();

	void update();
};

#endif // GAMEOBJECT_H