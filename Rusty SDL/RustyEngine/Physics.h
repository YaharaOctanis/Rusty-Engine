#ifndef RUSTYENGINE_PHYSICS_H
#define RUSTYENGINE_PHYSICS_H

#pragma once

#include "Vec2.h"
#include "Collider.h"
#include <vector>

namespace RustyEngine
{
	class Physics
	{
		friend class Rigidbody;
		// Add physics world to private (maybe just a BSP tree of colliders)
	protected:
		static std::vector<Collider*> colliders;

		static void addCollider(Collider* col);
		static void clearColliders();

	public:
		static Vec2 gravity;

	private:
		Physics() {};
		~Physics() {};
	};
}

#endif // RUSTYENGINE_PHYSICS_H