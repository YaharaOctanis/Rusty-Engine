#include "GameObject.h"



GameObject::GameObject()
{
	active = false;
	name = "Game object";
}

GameObject::GameObject(string name)
{
	active = false;
	this->name = name;
}


GameObject::~GameObject()
{
	if (!components.empty())
	{
		for (Component* component : components)
		{
			delete component;
		}
	}
}

void GameObject::update()
{
	if (!active)
		return;

	for(int i = 0; i < components.size(); i++)
		components[i]->update();
}
