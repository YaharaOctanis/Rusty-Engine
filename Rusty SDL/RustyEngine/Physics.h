#ifndef RUSTYENGINE_PHYSICS_H
#define RUSTYENGINE_PHYSICS_H

#pragma once

#include "Vec2.h"

namespace RustyEngine
{
	class Physics
	{
		// Add physics world to private (maybe just a BSP tree of colliders)

	public:
		static Vec2 gravity;

	private:
		Physics() {};
		~Physics() {};
	};
}

#endif // RUSTYENGINE_PHYSICS_H