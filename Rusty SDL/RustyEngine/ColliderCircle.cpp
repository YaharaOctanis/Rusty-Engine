#include "ColliderCircle.h"
#include "GameObject.h"
#include <cmath>
#include "Rigidbody.h"

// If you have trouble compiling with cmath PI, use this define (extracted from math.h)
//#define M_PI       3.14159265358979323846   // pi

namespace RustyEngine
{
	bool ColliderCircle::collisionCircleCircle(ColliderCircle *col)
	{
		// Get distance squared between objects
		float distance = game_object->transform.position.distanceToSqr(col->game_object->transform.position);
		float col_distance = (radius * game_object->transform.getScale().x) + (col->radius * col->game_object->transform.getScale().x);
		col_distance *= col_distance; // minimum distance for collision squared

		// If distance is smaller or equal to sum of radius, then we have a collision
		if (col_distance >= distance)
		{
			// Get from col to this vector (normal for rebound velocity change)
			Vec2 colNormal = game_object->transform.position - col->game_object->transform.position;
			colNormal.normalize();

			float relaxPercentage1, relaxPercentage2;

			// Get relax percentage
			if (col->rigidbody != nullptr)
			{
				relaxPercentage1 = col->mass / (mass + col->mass);
				relaxPercentage2 = mass / (mass + col->mass);
			}
			else
			{
				relaxPercentage1 = 1;
				relaxPercentage2 = 0;
			}

			// Calc relax distance
			float relaxDistance1 = (col_distance - distance) * relaxPercentage1;
			float relaxDistance2 = (col_distance - distance) * relaxPercentage2;

			// Relax objects
			game_object->transform.position = game_object->transform.position + (colNormal * relaxDistance1);
			col->game_object->transform.position = col->game_object->transform.position - (colNormal * relaxDistance2);


			// Calculate bounce
			float e = 0.5;
			float mass1inverse, mass2inverse = -1;
			float speedDifference = 0.0f;

			mass1inverse = 1 / mass;

			if (col->rigidbody != nullptr)
			{
				mass2inverse = 1 / col->mass;
				speedDifference = rigidbody->velocity.dot(colNormal) - col->rigidbody->velocity.dot(colNormal);
			}
			else
			{
				mass2inverse = 0;
				speedDifference = rigidbody->velocity.dot(colNormal);
			}

			// Distance to point of collision
			Vec2 vecToCol1 = colNormal * radius;
			Vec2 vecToCol2 = colNormal * -col->radius;

			// Moment of inertia compensated for size
			float rI1 = rigidbody->getMomentOfInertia() * powf(game_object->transform.getScale().x, 2.0f);
			float rI2 = col->rigidbody->getMomentOfInertia() *  powf(col->game_object->transform.getScale().x, 2.0f);

			// Calculate rotational factor
			float roll1 = powf(vecToCol1.dot(colNormal), 2) / rI1;
			float roll2 = 0;
			if(col->rigidbody != nullptr)
				roll2 = powf(vecToCol2.dot(colNormal), 2) / rI2;
			
			// Calculate impact force
			float impact = -(e + 1) * speedDifference / (mass1inverse + mass2inverse + roll1 + roll2);

			// Apply velocity and angular velocity to objects
			rigidbody->velocity = rigidbody->velocity + (colNormal * impact * mass1inverse);
			rigidbody->angular_velocity = rigidbody->angular_velocity + (vecToCol1.dot(colNormal * impact) / rI1);
			if (col->rigidbody != nullptr)
			{
				col->rigidbody->velocity = col->rigidbody->velocity - (colNormal * impact * mass2inverse);
				col->rigidbody->angular_velocity = col->rigidbody->angular_velocity - (vecToCol2.dot(colNormal * impact) / rI2);
			}

			// End
			return true;
		}

		return false;
	}



	float ColliderCircle::calculateMomentOfInertia(Vec2 point)
	{
		float I = 0.0f;

		I = (mass * powf(radius, 2)) / 2; // calculate circle's inertia
		I += mass * powf(game_object->transform.position.distanceTo(point), 2); // recalculate inertia for cutom hinge point

		return I;
	}

	bool ColliderCircle::collisionCheck(Collider *col)
	{
		// Ignore colliders on the same parent
		if (col->getGameObject() == this->game_object)
			return false;
		
		if (col->getType() == ColliderType::circle)
			return collisionCircleCircle(static_cast<ColliderCircle*>(col));

		return false;
	}


	void ColliderCircle::setRadius(float r)
	{
		radius = r;
		area = M_PI * pow(radius, 2);
	}


	float ColliderCircle::getRadius()
	{
		return radius;
	}


	ColliderCircle::ColliderCircle()
	{
		type = ColliderType::circle;
		setRadius(0.5f);
	}


	ColliderCircle::~ColliderCircle()
	{
	}
}

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