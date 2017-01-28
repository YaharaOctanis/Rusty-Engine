#ifndef RUSTYENGINE_COLLIDER_H
#define RUSTYENGINE_COLLIDER_H

#pragma once

#include "Component.h"
#include "Vec2.h"

namespace RustyEngine
{
	// Making collider inactive will only disable collisions, but will have no change on object's inertia or center of mass
	class Collider : public Component
	{
		friend class Rigidbody;
	protected:
		float area;	// Collider's surface area
		float mass; // Collider's mass (calculated by rigidbody, otherwise 0)

		// Calculates moment of inertia for given point
		virtual float calculateMomentOfInertia(Vec2 point) { return 0.0f; };

	public:
		Collider();
		~Collider();
	};
}

#endif // RUSTYENGINE_PHYSICS_H