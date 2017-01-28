#ifndef RUSTYENGINE_COMPONENT_H
#define RUSTYENGINE_COMPONENT_H

#pragma once

#include "Vec2.h"

namespace RustyEngine
{
	// Parent (wrapper) class for all components
	class Component
	{
		friend class GameObject;

	protected:
		bool active;
		GameObject* game_object;	// reference to parent game object

	public:
		const GameObject* getGameObject(); // returns read-only reference to a parent game object

		virtual void update() {};		// update function called once per render update (render loop)
		virtual void fixedUpdate() {};	// physics update function called once per physics update (physics loop)
		virtual void guiUpdate() {};	// called when game object is clicked (executes during render loop, right before update on component is called)

		virtual void onCollision(GameObject* g_obj, Vec2* cols) {};	// called every frame, for each object it collides with, takes game object and list of collision points as a parameter 
																	// (if object is colliding with 2 other objects, then this function will be called 2 times in that frame)

		Component();
		~Component();
	};
}

#endif // RUSTYENGINE_COMPONENT_H