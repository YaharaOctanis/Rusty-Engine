#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once

#include "Vec2.h"
#include <math.h>
#include <vector>

using namespace std;

// Object transformation structure, holds and manipulates local transformation matrix
class Transform
{
private:
	Transform* parent;
	vector<Transform*> children;
	Vec2 direction_x;
	Vec2 direction_y;
	float rotation;
	Vec2 scale;
public:
	Vec2 position;

public:
	Transform();
	Transform(Vec2 pos);
	Transform(Vec2 pos, float rot);
	Transform(Vec2 pos, float rot, Vec2 scale);
	~Transform();

	void setRotation(float rad);
	float getRotation();

	void setScale(Vec2 s);
	void setScale(float s);
	const Vec2& getScale();

	void setParent(Transform* p);
	Transform* getParent();

	vector<Transform*>::const_iterator getChildren();
	//Vec2 getDirection();
};

#endif // TRANSFORM_H