#ifndef RUSTYENGINE_VEC2_H
#define RUSTYENGINE_VEC2_H

#pragma once

namespace RustyEngine
{
	// Implementation of 2D vector
	class Vec2
	{
	public:
		float x;
		float y;

		Vec2();
		Vec2(float x, float y);
		~Vec2();

		void set(float x, float y);		// Set x and y
		void set(Vec2 &b);
		void set(Vec2 b);
		void normalize();				// Normalize vector to 1
		float length();					// Calculate and return length of this vector
		float sqareLength();			// Calculate and return squared length of this vector (faster than length)
		float dot(Vec2 &b);				// Returns dot product between this and given vector
		float cross(Vec2 &b);			// Returns cross product between this and given vector
		void lerp(Vec2 &b, float t);	// Linearly interpolates this vector toward given vector, limited by t
		float distanceTo(Vec2 &b);		// Calculate distance between this and given vector
		float distanceToSqr(Vec2 &b);	// Calculate distance squared between this and given vector (faster than distanceTo)
		float angleBetween(Vec2 &b);	// Calculate angle between this and given vector
		void rotate(float fi);			// Rotate vector for given angle
		Vec2 avg(Vec2 &b);				// Return average between vectors

		void operator=(Vec2 &b);		// Apply values of b to a
		Vec2 operator+(Vec2 &b);		// Add per vector member and return new Vec2
		Vec2 operator-(Vec2 &b);		// Subtract per vector member and return new Vec2
		Vec2 operator*(float m);		// Multiply all vector members by given value and return new Vec2
		Vec2 operator/(float d);		// Devide all vector members by given value and return new Vec2
		bool operator==(Vec2 &b);		// Return true if this and given vector have equal member values
		bool operator!=(Vec2 &b);		// Return true if this and given vector do not have equal member values
	};
}

#endif // RUSTYENGINE_VEC2_H