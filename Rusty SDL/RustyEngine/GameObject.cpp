#include "GameObject.h"

namespace RustyEngine
{
	// Constructors
	GameObject::GameObject()
	{
		// Use constant to check if we spawn objects active
		active = GAMEOBJECTS_START_ACTIVE;
		name = "Game object";
	}

	GameObject::GameObject(string name)
	{
		// Use constant to check if we spawn objects active
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
		for (int i = 0; i < components.size(); i++)
			if (components[i]->active)
				components[i]->update();
	}

	void GameObject::guiUpdate()
	{
		// Ignore object if inactive
		if (!active)
			return;

		// Call gui update function of every component
		for (int i = 0; i < components.size(); i++)
			if (components[i]->active)
				components[i]->guiUpdate();
	}

	void GameObject::addComponent(Component * component)
	{
		component->game_object = this;
		component->active = true;
		components.push_back(component);
	}
}