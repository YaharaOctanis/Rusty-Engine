#ifndef RUSTYENGINE_TRANSFORM_H
#define RUSTYENGINE_TRANSFORM_H

#pragma once

#include "Vec2.h"
#include <math.h>
#include <vector>


namespace RustyEngine
{
	using namespace std;

	// Object transformation structure, holds and manipulates local transformation matrix
	class Transform
	{
	private:
		Transform* parent;				// Parent of this gameobject (maybe transfer this control to game object?)
		vector<Transform*> children;	// List of this gameobjects's children (maybe transfer this control to game object?)
		Vec2 direction_x;				// World direction of this gameobject
		Vec2 direction_y;
		float rotation;					// Local rotation
		Vec2 scale;						// Local scale
	public:
		Vec2 position;					// Local position

	public:
		// Constructors
		Transform();
		Transform(Vec2 pos);
		Transform(Vec2 pos, float rot);
		Transform(Vec2 pos, float rot, Vec2 scale);

		// Destructor
		~Transform();

		void setRotation(float rad);	// Set rotation and update direction
		float getRotation();			// Get rotation

		void setScale(Vec2 s);			// Set direction and update direction (Vec2)
		void setScale(float s);			// Set direction and update direction (float)
		const Vec2& getScale();			// Get scale, returns Vec2 read-only reference

		void setParent(Transform* p);	// Change parent of this gameobject
		Transform* getParent();			// Get gameobject's parent

		vector<Transform*>::const_iterator getChildren();	// Return read-only list of children for this gameobject
		//Vec2 getDirection();
	};
}

#endif // RUSTYENGINE_TRANSFORM_H