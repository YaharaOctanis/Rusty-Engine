#include "Transform.h"


Transform::Transform()
{
	parent = nullptr;

	direction_x.Set(1, 0);
	direction_y.Set(0, 1);
	position.Set(0, 0);
	rotation = 0;
	scale.Set(1, 1);
}

Transform::Transform(Vec2 pos)
{
	parent = nullptr;

	direction_x.Set(1, 0);
	direction_y.Set(0, 1);
	position = pos;
	rotation = 0;
	scale.Set(1, 1);
}

Transform::Transform(Vec2 pos, float rot)
{
	parent = nullptr;

	direction_x.Set(1, 0);
	direction_y.Set(0, 1);
	position = pos;
	setRotation(rot);
	scale.Set(1, 1);
}

Transform::Transform(Vec2 pos, float rot, Vec2 scale)
{
	parent = nullptr;

	direction_x.Set(1, 0);
	direction_y.Set(0, 1);
	position = pos;
	setRotation(rot);
	setScale(scale);
}


Transform::~Transform()
{
}

void Transform::setRotation(float rad)
{
	int x = direction_x.x;

	rotation = rad;
	direction_x.x = (direction_x.x * cosf(rad)) - (direction_x.y * sinf(rad));
	direction_x.y = (x * sinf(rad)) + (direction_x.y * cosf(rad));

	x = direction_y.x;
	direction_y.x = (direction_y.x * cosf(rad)) - (direction_y.y * sinf(rad));
	direction_y.y = (x * sinf(rad)) + (direction_y.y * cosf(rad));
}

float Transform::getRotation()
{
	return rotation;
}

void Transform::setScale(Vec2 s)
{
	direction_x.Normalize();
	direction_y.Normalize();
	direction_x = direction_x * s.x;
	direction_y = direction_y * s.y;
}

Vec2 Transform::getScale()
{
	return scale;
}

void Transform::setParent(Transform* p)
{
	if (parent != nullptr)
		parent->children.remove(this);
	parent = p;
	parent->children.push_back(this);
}

Transform * Transform::getParent()
{
	return parent;
}

list<Transform*>::const_iterator Transform::getChildren()
{
	return children.cbegin;
}

/*
Vec2 Transform::getDirection()
{
	return direction;
}*/