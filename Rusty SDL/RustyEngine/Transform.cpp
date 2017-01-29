#include "Transform.h"

namespace RustyEngine
{
	// Constructors
	Transform::Transform()
	{
		parent = nullptr; // Start without parent :(

		// Set direction aligned with world axis
		direction_x.set(1, 0);
		direction_y.set(0, 1);

		// Set position to 0
		position.set(0, 0);

		// Set rotation to 0 (aligned with world axis)
		rotation = 0;

		// Set scale to 1
		scale.set(1, 1);
	}

	Transform::Transform(Vec2 pos)
	{
		parent = nullptr; // Start without parent

		// Set direction aligned with world axis
		direction_x.set(1, 0);
		direction_y.set(0, 1);

		// Set to given position
		position = pos;

		// Set rotation to 0 (aligned with world axis)
		rotation = 0;

		// Set scale to 1
		scale.set(1, 1);
	}

	Transform::Transform(Vec2 pos, float rot)
	{
		parent = nullptr; // Start without parent

		// Set direction aligned with world axis
		direction_x.set(1, 0);
		direction_y.set(0, 1);

		// Set to given position
		position = pos;

		// Set to given rotation (use setRotation to also recalculate direction vectors)
		setRotation(rot);

		// Set scale to 1
		scale.set(1, 1);
	}

	Transform::Transform(Vec2 pos, float rot, Vec2 scale)
	{
		parent = nullptr; // Start without parent

		// Set direction aligned with world axis
		direction_x.set(1, 0);
		direction_y.set(0, 1);

		// Set to given position
		position = pos;

		// Set to given rotation (use setRotation to also recalculate direction vectors)
		setRotation(rot);

		// Set to given scale (use setScale to also recalculate direction vectors)
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


	void Transform::rotate(float rad)
	{
		setRotation(rotation + rad);
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

	Vec2 Transform::getDirectionX()
	{
		return Vec2(direction_x);
	}


	Vec2 Transform::getDirectionY()
	{
		return Vec2(direction_y);
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


	// Return object scale (read-only)
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
}