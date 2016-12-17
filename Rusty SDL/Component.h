#ifndef COMPONENT_H
#define COMPONENT_H

#pragma once

// Parent (wrapper) class for all components
class Component
{
public:
	virtual void update() {};		// update function called once per render update (render loop)
	//virtual void fixedUpdate();	// physics update function called once per physics update (physics loop)
};

#endif // COMPONENT_H