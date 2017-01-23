#ifndef RUSTYENGINE_COLLIDER_H
#define RUSTYENGINE_COLLIDER_H

#pragma once

#include "Component.h"

namespace RustyEngine
{
	class Collider : public Component
	{
		friend class Rigidbody;
	protected:
		float area;	// Collider surface area

		// Calculates moment of inertia for given point and mass
		virtual float calculateMomentOfInertia(Vec2 point, float mass) {};

	public:
		Collider();
		~Collider();
	};
}

#endif // RUSTYENGINE_PHYSICS_H