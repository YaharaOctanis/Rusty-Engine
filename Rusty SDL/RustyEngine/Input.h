#ifndef RUSTYENGINE_INPUT_H
#define RUSTYENGINE_INPUT_H

#pragma once

#include "Vec2.h"

namespace RustyEngine
{
	class Input
	{
	private:
		Vec2 mouse_pos;

	public:
		Input();
		~Input();
	};
}

#endif // RUSTYENGINE_INPUT_H