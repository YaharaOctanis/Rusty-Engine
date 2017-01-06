#ifndef RUSTYENGINE_INPUT_H
#define RUSTYENGINE_INPUT_H

#pragma once

#include "Vec2.h"

namespace RustyEngine
{
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
		static Vec2 mouse_pos;
		static Vec2 touch_pos;
		static float mouse_left;
		static float mouse_right;
		static float mouse_middle;
		static float touch;

	public:
		static float getMouseDown(Mousebutton button);
		static const Vec2& getMousePos();
		static float getTouch();
		static const Vec2& getTouchPos();

		// TODO convert update private method and access it via friend class
		static void update();	// Handles input events (convert to only handle input events, instead of all)

	private:
		Input();
		~Input();
	};
}

#endif // RUSTYENGINE_INPUT_H