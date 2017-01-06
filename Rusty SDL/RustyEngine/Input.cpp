#include "Input.h"
#include <SDL.h>

namespace RustyEngine
{
	Vec2 Input::mouse_pos;
	Vec2 Input::touch_pos;
	float Input::mouse_left;
	float Input::mouse_right;
	float Input::mouse_middle;
	float Input::touch;

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

	const Vec2 & Input::getMousePos()
	{
		return mouse_pos;
	}

	float Input::getTouch()
	{
		return touch;
	}

	const Vec2 & Input::getTouchPos()
	{
		return touch_pos;
	}

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

	Input::Input()
	{
	}


	Input::~Input()
	{
	}
}