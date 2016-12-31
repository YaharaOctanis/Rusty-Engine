#ifndef RUSTYENGINE_COMPONENT_H
#define RUSTYENGINE_COMPONENT_H

#pragma once

namespace RustyEngine
{
	// Parent (wrapper) class for all components
	class Component
	{
	public:
		virtual void update() {};		// update function called once per render update (render loop)
		//virtual void fixedUpdate();	// physics update function called once per physics update (physics loop)
	};
}

#endif // RUSTYENGINE_COMPONENT_H