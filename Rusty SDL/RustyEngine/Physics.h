#ifndef RUSTYENGINE_PHYSICS_H
#define RUSTYENGINE_PHYSICS_H

#pragma once

#include "Vec2.h"
#include "Collider.h"
#include "ColliderCircle.h"
#include "ColliderHP.h"
#include "ColliderAAHP.h"
#include "ColliderRectangle.h"
#include "ColliderAAB.h"
#include <vector>
#include <SDL.h>

namespace RustyEngine
{
	class Physics
	{
		friend class Rigidbody;
		// Add physics world to private (maybe just a BSP tree of colliders)
	protected:
		static std::vector<Collider*> colliders;

	public:
		static Vec2 gravity;

		static Vec2* collisionLineLine(ColliderHP* col1, ColliderHP* col2);
		static Vec2* collisionLineLine(Vec2 a1, Vec2 a2, Vec2 b1, Vec2 b2);
		static Vec2* collisionLineHP(Vec2 a1, Vec2 a2, Vec2 b1, Vec2 b2);

		static bool collisionCircleHP(ColliderCircle *col1, ColliderHP *col2);
		static bool collisionCircleCircle(ColliderCircle *col1, ColliderCircle *col2);
		static bool collisionRectangleLine(ColliderRectangle *col1, ColliderHP* col2);
		static bool collisionRectangleLine(ColliderRectangle *col1, Vec2 p1, Vec2 p2);
		static bool collisionRectangleCircle(ColliderRectangle *col1, ColliderCircle *col2);
		static bool collisionRectangleRectangle(ColliderRectangle *col1, ColliderRectangle *col2);
		static bool collisionRectangleRectangle_old(ColliderRectangle *col1, ColliderRectangle *col2);

		static void getCorners(ColliderRectangle *col1, Vec2 corners[]);

		static void addCollider(Collider* col);
		static void clearColliders();

		static void debugDraw(Vec2 start, Vec2 end, SDL_Color c);

	private:
		Physics() {};
		~Physics() {};
	};
}

#endif // RUSTYENGINE_PHYSICS_H