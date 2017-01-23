#ifndef RUSTYENGINE_COMPONENT_H
#define RUSTYENGINE_COMPONENT_H

#pragma once

namespace RustyEngine
{
	// Parent (wrapper) class for all components
	class Component
	{
		friend class GameObject;

	protected:
		bool active = false;
		GameObject* game_object;	// reference to parent game object

	public:
		virtual void update() {};		// update function called once per render update (render loop)
		virtual void fixedUpdate() {};	// physics update function called once per physics update (physics loop)
		virtual void guiUpdate() {};	// called when game object is clicked (executes during render loop, right before update on component is called)
	};
}

#endif // RUSTYENGINE_COMPONENT_H