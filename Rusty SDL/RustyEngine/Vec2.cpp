#include "Vec2.h"
#include "math.h"

namespace RustyEngine
{
	// Constructors & destructors

	Vec2::Vec2()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vec2::Vec2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vec2::~Vec2() // default destructor
	{
	}



	// Methods

	// Set x and y
	void Vec2::set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	// Normalize vector to 1
	void Vec2::normalize()
	{
		float d = this->length();
		x = x / d;
		y = y / d;
	}

	// Calculate and return length of this vector
	float Vec2::length()
	{
		return sqrtf((this->x * this->x) + (this->y * this->y));
	}

	// Calculate and return squared length of this vector (faster than length)
	float Vec2::sqareLength()
	{
		return (this->x * this->x) + (this->y * this->y);
	}

	// Returns dot product between this and given vector
	float Vec2::dot(Vec2 &b)
	{
		return x * b.x + y * b.y;
	}

	// Returns cross product between this and given vector
	float Vec2::cross(Vec2 & b)
	{
		return x * b.y - y * b.x;
	}

	// Linearly interpolates this vector toward given vector, limited by t
	void Vec2::lerp(Vec2 &b, float t)
	{
		x = x + ((b.x - x) * t);
		y = y + ((b.y - y) * t);
	}

	// Calculate distance between this and given vector
	float Vec2::distanceTo(Vec2 &b)
	{
		return sqrtf(((b.x - x) * (b.x - x)) + ((b.y - y) * (b.y - y)));
	}

	// Calculate angle between this and given vector
	float Vec2::angleBetween(Vec2 &b)
	{
		return acosf(this->dot(b) / (this->length() * b.length()));
	}



	// Operator overloads

	void Vec2::operator=(Vec2 & b)
	{
		x = b.x;
		y = b.y;
		//return Vec2(b.x, b.y);
	}

	// Add per vector member and return new Vec2
	Vec2 Vec2::operator+(Vec2 &b)
	{
		return Vec2(x + b.x, y + b.y);
	}

	// Subtract per vector member and return new Vec2
	Vec2 Vec2::operator-(Vec2 & b)
	{
		return Vec2(x - b.x, y - b.y);
	}

	// Multiply all vector members by given value and return new Vec2
	Vec2 Vec2::operator*(float m)
	{
		return Vec2(x*m, y*m);
	}

	// Devide all vector members by given value and return new Vec2
	Vec2 Vec2::operator/(float d)
	{
		return Vec2(x / d, y / d);
	}

	// Return true if this and given vector have equal member values
	bool Vec2::operator==(Vec2 & b)
	{
		return (x == b.x) && (y == b.y);
	}

	// Return true if this and given vector do not have equal member values
	bool Vec2::operator!=(Vec2 & b)
	{
		return (x != b.x) || (y != b.y);
	}


	// Static Methods

	// None :(
}