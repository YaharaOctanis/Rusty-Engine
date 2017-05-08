#ifndef RUSTYENGINE_COLLIDER_H
#define RUSTYENGINE_COLLIDER_H

#pragma once

#include "Component.h"
#include "Vec2.h"

namespace RustyEngine
{
	enum ColliderType : int
	{
		none = 0,
		circle = 1,
		rectangle = 2,
		hp = 3,
		aahp = 4,
		aab = 5
	};
	
	// Making collider inactive will only disable collisions, but will have no change on object's inertia or center of mass
	class Collider : public Component
	{
		friend class Rigidbody;
		friend class Physics;
	protected:
		Rigidbody* rigidbody;
		float area;	// Collider's surface area
		float mass; // Collider's mass (calculated by rigidbody, otherwise 0)
		ColliderType type;

		// Calculates moment of inertia for given point
		virtual float calculateMomentOfInertia(Vec2 point) { return 0.0f; };
		virtual bool collisionCheck(Collider *col) { return false; };

	public:
		bool isTrigger;
		Vec2 last_col_normal;
		bool collided;
		const ColliderType getType();

		Collider();
		~Collider();
	};
}

#endif // RUSTYENGINE_PHYSICS_H