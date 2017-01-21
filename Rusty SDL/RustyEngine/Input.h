#ifndef RUSTYENGINE_INPUT_H
#define RUSTYENGINE_INPUT_H

#pragma once

#include "Vec2.h"

namespace RustyEngine
{
	// Mousebutton enum (use it to specify mouse buttons in mouse input functions)
	enum Mousebutton : int
	{
		left = 0,
		middle = 1,
		right = 2
	};

	// Make event class and make sure this class extends it
	class Input
	{
	private:
		static Vec2 mouse_pos;		// Mouse position
		static Vec2 touch_pos;		// Touch position (on touchscreen devices)
		static float mouse_left;	// Is left mouse button clicked (how much? acceleration can be used for non-analog inputs)
		static float mouse_right;	// Is right mouse button clicked (how much?)
		static float mouse_middle;  // Is middle mouse button clicked (how much?)
		static float touch;			// Has anyone touched screen? (how much? acceleration can be used for non-analog inputs)

	public:
		static float getMouseDown(Mousebutton button);	// Return how much mouse button is clicked (for single button only)
		static const Vec2& getMousePos();				// Get absolute mouse position (read-only)
		static float getTouch();						// Retrun if anyone has touched the screen (and how much)
		static const Vec2& getTouchPos();				// Get absolute touch position (read-only)

		// TODO convert update to private method and access it via friend class
		static void update();		// Handles input events (convert to only handle input events, instead of all events)

	
	private:
		// Cannot construct static only class (well... you can, but is pointless)
		Input();
		~Input();
	};
}

#endif // RUSTYENGINE_INPUT_H