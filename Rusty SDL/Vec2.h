#ifndef VEC2_H
#define VEC2_H

#pragma once

class Vec2
{
public:
	float x;
	float y;

	Vec2();
	Vec2(float x, float y);
	~Vec2();

	void Set(float x, float y);
	void Normalize();
	float Length();					// Calculate and return length of this vector
	float Dot(Vec2 &b);				// Returns dot product between this and given vector
	void Lerp(Vec2 &b, float t);	// Linearly interpolates this vector toward given vector, limited by t
	float DistanceTo(Vec2 &b);		// Calculate distance between this and given vector

	Vec2 operator+(Vec2 &b);		// Add per vector member and return new Vec2
	Vec2 operator-(Vec2 &b);		// Subtract per vector member and return new Vec2
	Vec2 operator*(float m);		// Multiply all vector members by given value and return new Vec2
	Vec2 operator/(float d);		// Devide all vector members by given value and return new Vec2
	bool operator==(Vec2 &b);		// Return true if this and given vector have equal member values
	bool operator!=(Vec2 &b);		// Return true if this and given vector do not have equal member values
};

#endif // VEC2_H