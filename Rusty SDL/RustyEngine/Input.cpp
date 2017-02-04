#include "Input.h"
#include <SDL.h>

namespace RustyEngine
{
	Vec2 Input::mouse_pos;		// Mouse position
	Vec2 Input::touch_pos;		// Touch position (on touchscreen devices)
	float Input::mouse_left;	// Is left mouse button clicked (how much? acceleration can be used for non-analog inputs)
	float Input::mouse_right;	// Is right mouse button clicked (how much?)
	float Input::mouse_middle;	// Is middle mouse button clicked (how much?)
	float Input::touch;			// Has anyone touched screen? (how much? acceleration can be used for non-analog inputs)


	// Return how much mouse button is clicked (for single button only)
	float Input::getMouseDown(Mousebutton button)
	{
		switch (button)
		{
			case left: return mouse_left;
			case middle: return mouse_middle;
			case right: return mouse_right;
		}
		return 0.0f;
	}


	// Get absolute mouse position (read-only)
	const Vec2 & Input::getMousePos()
	{
		return mouse_pos;
	}


	// Retrun if anyone has touched the screen (and how much)
	float Input::getTouch()
	{
		return touch;
	}


	// Get absolute touch position (read-only)
	const Vec2 & Input::getTouchPos()
	{
		return touch_pos;
	}

	// TODO convert update to private method and access it via friend class
	// Handles input events (convert to only handle input events, instead of all events)
	void Input::update()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))	// Grab input events
		{
			switch (event.type)
			{
				// Handle mouse button press
				case SDL_MOUSEBUTTONDOWN:
					mouse_pos.set(event.motion.x, event.motion.y);
					
					if (event.button.button & SDL_BUTTON_LMASK)
						mouse_left = 1.0f;
					if (event.button.button & SDL_BUTTON_MMASK)
						mouse_middle = 1.0f;
					if (event.button.button & SDL_BUTTON_RMASK)
						mouse_right = 1.0f;
					break;

				// Handle mouse button release
				case SDL_MOUSEBUTTONUP:
					mouse_pos.set(event.motion.x, event.motion.y);

					if (event.button.button & SDL_BUTTON_LMASK)
						mouse_left = 0.0f;
					if (event.button.button & SDL_BUTTON_MMASK)
						mouse_middle = 0.0f;
					if (event.button.button & SDL_BUTTON_RMASK)
						mouse_right = 0.0f;
					break;

				// Handle mouse movement
				case SDL_MOUSEMOTION:
					mouse_pos.set(event.motion.x, event.motion.y);
					break;

				// Handle finger press
				case SDL_FINGERDOWN:
					touch_pos.set(event.tfinger.x, event.tfinger.y);
					touch = 1.0f;
					break;

				// Handle finger release
				case SDL_FINGERUP:
					touch_pos.set(event.tfinger.x, event.tfinger.y);
					touch = 0.0f;
					break;

				// Handle finger movement
				case SDL_FINGERMOTION:
					touch_pos.set(event.tfinger.x, event.tfinger.y);
					break;


				// Special system events
				case SDL_QUIT:
					exit(0);
			}
		}
	}




	// Cannot construct static only class (well... you can, but is pointless)
	Input::Input()
	{
	}

	Input::~Input()
	{
	}
}