#ifndef RUSTYENGINE_GAMEOBJECT_H
#define RUSTYENGINE_GAMEOBJECT_H

#pragma once

#include "Transform.h"
#include "Component.h"
#include "RustyConstants.h"
#include <vector>
#include <string>

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

		void start();								// Start function called when first starting the level
		void pause();								// Pause function called when level is paused
		void resume();								// Resume function called when level is resumed
		void update();								// Update function called once per render update (render loop)
		void guiUpdate();							// TODO rethink this function
		void onCollision(GameObject* g_obj, Vec2 col_normal);
		void addComponent(Component* component);	// Add component to the game object

		template <class T> T* getComponent();
		void* getComponent(const type_info &type);
	};

	template<class T>
	inline T* GameObject::getComponent()
	{
		for (unsigned int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]) == typeid(T))
				return static_cast<T*>(components[i]);
		}
		return nullptr;
	}
}

#endif // RUSTYENGINE_GAMEOBJECT_H