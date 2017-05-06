#include "GameObject.h"
#include <typeinfo>
#include "Game.h"

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
		/* redundant, since we now have pixel perfect rendering
		// Clamp to pixel perfect starting position
		if (START_PIXEL_PERFECT)
		{
			Vec2 temp_vec(1, 0), temp2_vec(0, 0);
			float pixel_size = (Game::world.screenToWorldSpace(temp_vec) - Game::world.screenToWorldSpace(temp2_vec)).x;
			pixel_size *= RENDER_SCALE;

			if (pixel_size != 0)
			{
				transform.position.x = transform.position.x - (fmodf(transform.position.x / pixel_size, 1) * pixel_size);
				transform.position.y = transform.position.y - (fmodf(transform.position.y / pixel_size, 1) * pixel_size);
			}
		}*/		

		// Call start on every component, even if inactive
		for (unsigned int i = 0; i < components.size(); i++)
				components[i]->start();
	}


	// Pause function called when level is paused
	void GameObject::pause()
	{
		// Call pause on every component, even if inactive
		for (unsigned int i = 0; i < components.size(); i++)
			components[i]->pause();
	}


	// Resume function called when level is resumed
	void GameObject::resume()
	{
		// Call resume on every component, even if inactive
		for (unsigned int i = 0; i < components.size(); i++)
			components[i]->resume();
	}


	// Update function called once per render update (render loop)
	void GameObject::update()
	{
		// Ignore object if inactive
		if (!active)
			return;

		// Call update function on every component
		for (unsigned int i = 0; i < components.size(); i++)
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
		for (unsigned int i = 0; i < components.size(); i++)
			if (components[i]->active)
				components[i]->guiUpdate();
	}

	void GameObject::onCollision(GameObject * g_obj, Vec2 col_normal)
	{
		// Ignore object if inactive
		if (!active)
			return;

		// Call onCollision function on every component
		for (unsigned int i = 0; i < components.size(); i++)
			if (components[i]->active)
				components[i]->onCollision(g_obj, col_normal);
	}


	// Add component to the game object
	void GameObject::addComponent(Component * component)
	{
		component->game_object = this;
		component->active = true;
		components.push_back(component);
	}
	void * GameObject::getComponent(const type_info &type)
	{
		for (unsigned int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]) == type)
				return components[i];
		}
		return nullptr;
	}
}