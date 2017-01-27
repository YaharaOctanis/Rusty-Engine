#ifndef RUSTYENGINE_COLLIDERCIRCLE_H
#define RUSTYENGINE_COLLIDERCIRCLE_H

#pragma once

#define _USE_MATH_DEFINES
#include "Collider.h"

namespace RustyEngine
{
	class ColliderCircle : public Collider
	{
	protected:
		float radius;		// If you change collider radius, you have to recalculate moment of inertia for this object's rigidbody

		// Calculates moment of inertia for given point
		float calculateMomentOfInertia(Vec2 point);

	public:
		void setRadius(float r);
		float getRadius();

		ColliderCircle();
		~ColliderCircle();
	};
}

#endif // RUSTYENGINE_COLLIDERCIRCLE_H