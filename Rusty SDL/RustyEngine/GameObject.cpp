#include "GameObject.h"

namespace RustyEngine
{
	// Constructors
	GameObject::GameObject()
	{
		// Use constant to check if we spawn objects active
		active = GAMEOBJECTS_START_ACTIVE;
		name = "Game object";
		tag = "";
	}

	GameObject::GameObject(string name)
	{
		// Use constant to check if we spawn objects active
		active = GAMEOBJECTS_START_ACTIVE;
		this->name = name;
		tag = "";
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


	// Start function called when first starting the level
	void GameObject::start()
	{
		// Call start on every component, even if inactive
		for (int i = 0; i < components.size(); i++)
				components[i]->start();
	}


	// Pause function called when level is paused
	void GameObject::pause()
	{
		// Call pause on every component, even if inactive
		for (int i = 0; i < components.size(); i++)
			components[i]->pause();
	}


	// Resume function called when level is resumed
	void GameObject::resume()
	{
		// Call resume on every component, even if inactive
		for (int i = 0; i < components.size(); i++)
			components[i]->resume();
	}


	// Update function called once per render update (render loop)
	void GameObject::update()
	{
		// Ignore object if inactive
		if (!active)
			return;

		// Call update function on every component
		for (int i = 0; i < components.size(); i++)
			if (components[i]->active)
				components[i]->update();
	}


	// TODO rethink this function
	void GameObject::guiUpdate()
	{
		// Ignore object if inactive
		if (!active)
			return;

		// Call gui update function on every component
		for (int i = 0; i < components.size(); i++)
			if (components[i]->active)
				components[i]->guiUpdate();
	}

	void GameObject::onCollision(GameObject * g_obj)
	{
		// Ignore object if inactive
		if (!active)
			return;

		// Call onCollision function on every component
		for (int i = 0; i < components.size(); i++)
			if (components[i]->active)
				components[i]->onCollision(g_obj, nullptr);
	}


	// Add component to the game object
	void GameObject::addComponent(Component * component)
	{
		component->game_object = this;
		component->active = true;
		components.push_back(component);
	}
}