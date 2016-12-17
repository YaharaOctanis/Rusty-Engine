#include "GameObject.h"


// Constructors
GameObject::GameObject()
{
	// Use constant to check if start active
	active = GAMEOBJECTS_START_ACTIVE;
	name = "Game object";
}

GameObject::GameObject(string name)
{
	active = GAMEOBJECTS_START_ACTIVE;
	this->name = name;
}

// Destructor
GameObject::~GameObject()
{
	// If there are components attached to the game object
	if (!components.empty())
	{
		// Delete those components first, or suffer memory leaks
		for (Component* component : components)
		{
			delete component;
		}
	}
}

// Update function called once per render update (render loop)
void GameObject::update()
{
	// Ignore object if inactive
	if (!active)
		return;

	// Call update function of every component
	for(int i = 0; i < components.size(); i++)
		components[i]->update();
}
