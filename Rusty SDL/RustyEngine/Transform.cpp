#include "Transform.h"

using namespace RustyEngine;
// Constructors
Transform::Transform()
{
	parent = nullptr; // Start without parent :(

	// Set direction aligned with world axis
	direction_x.set(1, 0);
	direction_y.set(0, 1);

	// Set position to 0
	position.set(0, 0);

	// Set rotation to 0
	rotation = 0;

	// Set scale to 1
	scale.set(1, 1);
}

Transform::Transform(Vec2 pos)
{
	parent = nullptr;

	direction_x.set(1, 0);
	direction_y.set(0, 1);
	position = pos;
	rotation = 0;
	scale.set(1, 1);
}

Transform::Transform(Vec2 pos, float rot)
{
	parent = nullptr;

	direction_x.set(1, 0);
	direction_y.set(0, 1);
	position = pos;
	setRotation(rot);
	scale.set(1, 1);
}

Transform::Transform(Vec2 pos, float rot, Vec2 scale)
{
	parent = nullptr;

	direction_x.set(1, 0);
	direction_y.set(0, 1);
	position = pos;
	setRotation(rot);
	setScale(scale);
}


// Destructor
Transform::~Transform()
{
	// TODO remove this item from parent if parent != null
	parent = nullptr;	// Remove parent
	if (!children.empty())
	{
		// Remove all children recursively, if they exist
		for (Transform* child : children)
		{
			delete child;
		}
	}
}


// Set object rotation
void Transform::setRotation(float rad)
{
	int x = direction_x.x;

	// Set rotation, then update world direction vectors
	rotation = rad;
	direction_x.x = (direction_x.x * cosf(rad)) - (direction_x.y * sinf(rad));
	direction_x.y = (x * sinf(rad)) + (direction_x.y * cosf(rad));

	x = direction_y.x;
	direction_y.x = (direction_y.x * cosf(rad)) - (direction_y.y * sinf(rad));
	direction_y.y = (x * sinf(rad)) + (direction_y.y * cosf(rad));
}

// Return object rotation
float Transform::getRotation()
{
	return rotation;
}

// Set object scale (Vec2)
void Transform::setScale(Vec2 s)
{
	// Normalize direction first
	direction_x.normalize();
	direction_y.normalize();

	// Multiply with scale vector
	direction_x = direction_x * s.x;
	direction_y = direction_y * s.y;

	// Then set scale values
	scale.set(s.x, s.y);
}

// Set object scale (float)
void Transform::setScale(float s)
{
	// Normalize direction first
	direction_x.normalize();
	direction_y.normalize();

	// Multiply with scale value
	direction_x = direction_x * s;
	direction_y = direction_y * s;

	// Then set scale values
	scale.set(s, s);
}

// Return object scale
const Vec2& Transform::getScale()
{
	return scale;
}

// Set object parent
void Transform::setParent(Transform* p)
{
	/*
	// Remove yourself from children list of current parent
	if (parent != nullptr)
		parent->children.remove(this);
	// Set parent
	parent = p;
	// Then add yourself to children list of new parent
	parent->children.push_back(this);*/
}

// Get object parent
Transform * Transform::getParent()
{
	return parent;
}

// Return read-only array iterator of object's children
vector<Transform*>::const_iterator Transform::getChildren()
{
	return children.cbegin();
}

/*
Vec2 Transform::getDirection()
{
	return direction;
}*/