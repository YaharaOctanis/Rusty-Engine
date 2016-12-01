#include "Vec2.h"
#include "math.h"


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


Vec2::~Vec2()
{
}


// Methods

void Vec2::Set(float x, float y)
{
	this->x = x;
	this->y = y;
}


void Vec2::Normalize()
{
	float d = this->Length();
	x = x / d;
	y = y / d;
}


float Vec2::Length()
{
	return sqrtf((this->x * this->x) + (this->y * this->y));
}

float Vec2::Dot(Vec2 &b)
{
	return x * b.x + y * b.y;
}


void Vec2::Lerp(Vec2 &b, float t)
{
	x = x + ((b.x - x) * t);
	y = y + ((b.y - y) * t);
}

float Vec2::DistanceTo(Vec2 &b)
{
	return sqrtf(((b.x - x) * (b.x - x)) + ((b.y - y) * (b.y - y)));
}


// Operator overloads

Vec2 Vec2::operator+(Vec2 &b)
{
	return Vec2(x + b.x, y + b.y);
}

Vec2 Vec2::operator-(Vec2 & b)
{
	return Vec2(x - b.x, y - b.y);
}

Vec2 Vec2::operator*(float m)
{
	return Vec2(x*m, y*m);
}

Vec2 Vec2::operator/(float d)
{
	return Vec2(x/d, y/d);
}

bool Vec2::operator==(Vec2 & b)
{
	return (x == b.x) && (y == b.y);
}

bool Vec2::operator!=(Vec2 & b)
{
	return (x != b.x) || (y != b.y);
}


// Static Methods

// None :(