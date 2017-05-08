#ifndef RUSTYENGINE_RIGIDBODY_H
#define RUSTYENGINE_RIGIDBODY_H

#pragma once

#define _USE_MATH_DEFINES
#include "Vec2.h"
#include "Component.h"
#include "Collider.h"
#include <vector>

namespace RustyEngine
{
	class Rigidbody : public Component
	{
	private:
		Vec2 force;
		float torque;
		std::vector<Collider*> colliders;	// List of attached colliders

		Vec2 center_of_mass;
		float moment_of_inertia;
		float area;
		bool updateInertia;

		void applyDrag();		// Actually, it's just simple velocity damping
		void calculateMomentOfInertia();
		void calculateArea();
		void calculateCenterOfMass();


	protected:
		void collisionCheck();

	public:
		float drag;
		float angular_drag;
		float mass;
		Vec2 velocity;
		float angular_velocity;
		bool use_gravity;

		bool freeze_x;			// If true, rigidbody will not update object position on x axis and will set velocity x to 0
		bool freeze_y;			// If true, rigidbody will not update object position on y axis and will set velocity y to 0
		bool freeze_rotation;	// If true, rigidbody will not update object rotation and will set rotational velocity to 0

		Rigidbody();
		~Rigidbody();

		void update() { fixedUpdate(); }; // JUST FOR TESTING, NORMALLY THIS FUNCTION SHOULD NOT EXIST, EVERYTHING SHOULD BE DONE IN FIXED UPDATE
		void fixedUpdate();
		void onCollision(GameObject* g_obj, Vec2* cols);

		void addForce(Vec2 force);
		void addForceAtPosition(Vec2 force, Vec2 position);

		void addCollider(Collider *col);	// Adds collider to the composite (this function will also add the collider to game object's list of components)
		float getMomentOfInertia();
	};
}

#endif // RUSTYENGINE_RIGIDBODY_H



/*

point - point where force is applied
center - object's center of mass
distance - distance between center and point
angle - angle between, center to point line, and line represented by direction of force

torque = N * distance * sin(angle)
angular_velocity = torque * delta_t / moment_of_inertia

Moments of inertia for 2D only:
circle - I = (mass * radius^2) / 2
rectangle (center) - I = (mass/12) * (height^2 + width^2)
rectangle (on w edge) - I = (mass/12) * (4*(height^2) + width^2)

custom rotato point - I = I + (mass * center_to_hinge_point_distante^2)
composite_I = I_1 + I_2 + ... + I_n (after custom rotato point calculation)

For composite object, calculate new mathematical center of mass for composite. Give each object mass relative to it's surface area, so that their sum will equal mass of composite. 
Move hinge point to the composite center of mass. Then apply new forces normally, but add them to each object independently (you can do this for torque only and apply velocity to the composite itself).


Center of mass for a composite shape

object_n = distance_from_edge * object_n_surface_area

center_of_mass = (object_1 + object_2 + ... + object_n) / (object_1_surface_area + object_2_surface_area + ... + object_n_surface_area)
Do this for both x and y edges. This will tell you how far from the given edge, is the center of mass.

*/