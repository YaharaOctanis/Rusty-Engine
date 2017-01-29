#ifndef RUSTYENGINE_COLLIDERHP_H
#define RUSTYENGINE_COLLIDERHP_H

#pragma once

#define _USE_MATH_DEFINES
#include "Collider.h"

namespace RustyEngine
{
	class ColliderHP : public Collider
	{
		friend class ColliderCircle;
	protected:
		Vec2 direction;

		// Line without mass or area, doesn't have moment of inertia
		float calculateMomentOfInertia(Vec2 point) { return 0.0f; }

		// Collision check
		bool collisionCheck(Collider *col);

	public:
		float length;

		ColliderHP();
		ColliderHP(Vec2 dir);
		~ColliderHP();

		Vec2 getWorldMin();
		Vec2 getWorldMax();
	};
}

#endif // RUSTYENGINE_COLLIDERHP_H