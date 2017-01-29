#ifndef RUSTYENGINE_COLLIDERCIRCLE_H
#define RUSTYENGINE_COLLIDERCIRCLE_H

#pragma once

#define _USE_MATH_DEFINES
#include "Collider.h"
#include "ColliderAAHP.h"
#include "ColliderHP.h"

namespace RustyEngine
{
	class ColliderCircle : public Collider
	{
	protected:
		float radius;		// If you change collider radius, you have to recalculate moment of inertia for this object's rigidbody

		// Calculates moment of inertia for given point
		float calculateMomentOfInertia(Vec2 point);
		bool collisionCheck(Collider *col);

	public:
		void setRadius(float r);
		float getRadius();

		static bool collisionCircleHP(ColliderCircle *col1, ColliderHP *col2);
		static bool collisionCircleCircle(ColliderCircle *col1, ColliderCircle *col2);

		ColliderCircle();
		~ColliderCircle();
	};
}

#endif // RUSTYENGINE_COLLIDERCIRCLE_H