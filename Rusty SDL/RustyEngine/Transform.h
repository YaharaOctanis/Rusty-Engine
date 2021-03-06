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

		void rotate(float rad);			// Rotate for given degrees
		void setRotation(float rad);	// Set rotation and update direction
		float getRotation();			// Get rotation
		Vec2 getDirectionX();			// Get direction x
		Vec2 getDirectionY();			// Get direction y

		void setScale(Vec2 s);			// Set object scale and update direction (Vec2)
		void setScale(float s);			// Set object scale and update direction (float)
		void setScale(float x, float y);// Set object scale and update direction (float, float)
		const Vec2& getScale();			// Get scale, returns Vec2 read-only reference

		void setParent(Transform* p);	// Change parent of this gameobject
		Transform* getParent();			// Get gameobject's parent
		void removeParent();			// Remove object from parent

		vector<Transform*>::const_iterator getChildren();	// Return read-only list of children for this gameobject
		//Vec2 getDirection();
	};
}

#endif // RUSTYENGINE_TRANSFORM_H