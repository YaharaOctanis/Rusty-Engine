#ifndef RUSTYENGINE_COLLIDERRECTANGLE_H
#define RUSTYENGINE_COLLIDERRECTANGLE_H

#pragma once

#include "Collider.h"

namespace RustyEngine
{
	class ColliderRectangle : public Collider
	{
	protected:
		Vec2 size;

		// Calculates moment of inertia for given point
		float calculateMomentOfInertia(Vec2 point);
		void calculateArea();
		bool collisionCheck(Collider *col);

	public:
		ColliderRectangle();
		~ColliderRectangle();

		void setSize(Vec2 s);				// Set collider's size with vec2
		void setSize(float s);				// Set equal size to both width and heigh, which will create a square
		void setSize(float w, float h);		// Set collider's size with floats
		
		const Vec2& getSize();				// Get collider's size as Vec2 (read-only)
	};
}

#endif // RUSTYENGINE_COLLIDERRECTANGLE_H