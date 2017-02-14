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
		GameObject* game_object;	// reference to parent game object

	public:
		bool active;

		const GameObject* getGameObject(); // returns read-only reference to a parent game object


		virtual void start() {};		// called when starting the level
		virtual void pause() {};		// called when level is paused
		virtual void resume() {};		// called when level is resumed
		virtual void update() {};		// update function called once per render update (render loop)
		virtual void fixedUpdate() {};	// physics update function called once per physics update (physics loop)
		virtual void guiUpdate() {};	// called when game object is clicked (executes during render loop, right before update on component is called)

		virtual void onCollision(GameObject* g_obj, Vec2 col_normal) {};	// called every frame, for each object it collides with, takes game object and list of collision points as a parameter 
																			// (if object is colliding with 2 other objects, then this function will be called 2 times in that frame)

		Component();
		virtual ~Component();
	};
}

#endif // RUSTYENGINE_COMPONENT_H