#include "ColliderCircle.h"
#include "ColliderAAHP.h"
#include "GameObject.h"
#include <cmath>
#include "Rigidbody.h"

// If you have trouble compiling with cmath PI, use this define (extracted from math.h)
//#define M_PI       3.14159265358979323846   // pi

namespace RustyEngine
{
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
		
		switch (col->getType())
		{
			case ColliderType::circle : return collisionCircleCircle(this, static_cast<ColliderCircle*>(col));
			case ColliderType::aahp: return collisionCircleHP(this, static_cast<ColliderAAHP*>(col));
			case ColliderType::hp: return collisionCircleHP(this, static_cast<ColliderHP*>(col));
		}
		

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

	bool ColliderCircle::collisionCircleHP(ColliderCircle * col1, ColliderHP * col2)
	{		
		Vec2 worldMin;
		Vec2 worldMax;

		if (col2->getType() == ColliderType::aahp)
		{
			ColliderAAHP* col3 = static_cast<ColliderAAHP*>(col2);
			worldMin = col3->getWorldMin();
			worldMax = col3->getWorldMax();
		}
		else
		{
			worldMin = col2->getWorldMin();
			worldMax = col2->getWorldMax();
		}

		Vec2 localMin = worldMin - col1->game_object->transform.position;
		Vec2 localMax = worldMax - col1->game_object->transform.position;
		Vec2 dir = localMax - localMin;

		float line_len_sqr = powf(col2->length * col2->game_object->transform.getScale().x, 2);
		float dot2_min_dir = 2 * dir.dot(localMin);

		float delta = powf(dot2_min_dir, 2) - (4 * line_len_sqr * (localMin.sqareLength() - powf(col1->radius * col1->game_object->transform.getScale().x, 2)));
		Vec2 col_point;
		float overlap = 0;
		float dist_col2min;
		float dist_col2max;

		if (delta == 0) // One collision point
		{
			float point = -dot2_min_dir / (2 * line_len_sqr);
			col_point = worldMin + (dir * point);
			overlap = 0;

			dist_col2min = col_point.distanceToSqr(worldMin);
			dist_col2max = col_point.distanceToSqr(worldMax);

			// If there is a single collision point and it is not on line, then no collision
			if (dist_col2min > line_len_sqr || dist_col2max > line_len_sqr)
				return false;
		}
		else if (delta > 0) // Two collision points
		{
			float sqrt_delta = sqrtf(delta);

			float point1 = (-dot2_min_dir + sqrt_delta) / (2 * line_len_sqr);
			float point2 = (-dot2_min_dir - sqrt_delta) / (2 * line_len_sqr);
			float point_mid = -dot2_min_dir / (2 * line_len_sqr);

			// Calculate collision points in world space
			Vec2 world_col_point1 = worldMin + (dir * point1);
			Vec2 world_col_point2 = worldMin + (dir * point2);
			Vec2 world_col_point_mid = worldMin + (dir * point_mid);

			// Calculate midway point between collision points
			col_point = world_col_point_mid;
			//col_point = world_col_point1 + dir * (world_col_point1.distanceTo(world_col_point2) / 2);
			//overlap = (radius * game_object->transform.getScale().x) - (game_object->transform.position - col_point).length();

			float col_length = col_point.distanceToSqr(world_col_point1);
			dist_col2min = col_point.distanceToSqr(worldMin);
			dist_col2max = col_point.distanceToSqr(worldMax);

			// Check if collision point lies on line
			if (dist_col2min > line_len_sqr && dist_col2max > line_len_sqr) // Not on line
				return false;
			else if (dist_col2min <= line_len_sqr && dist_col2max > line_len_sqr) // Off the line, but col point could be world min
			{
				if (dist_col2min <= col_length)
					col_point = worldMin;
				else
					return false;
			}
			else if (dist_col2min > line_len_sqr && dist_col2max <= line_len_sqr) // Off the line, but col point could be world max
			{
				if (dist_col2max <= col_length)
					col_point = worldMax;
				else
					return false;
			}
			// Else on line normally
		}
		else // No collision
			return false;

		// Calculate overlap, when there are two collision points
		if (delta > 0)
			overlap = (col1->radius * col1->game_object->transform.getScale().x) - (col1->game_object->transform.position - col_point).length();

		// Calculate collision normal
		Vec2 colNormal = col1->game_object->transform.position - col_point;
		colNormal.normalize();

		float relaxPercentage1, relaxPercentage2;

		// Get relax percentage
		if (col2->mass > 0)
		{
			relaxPercentage1 = col2->mass / (col1->mass + col2->mass);
			relaxPercentage2 = col1->mass / (col1->mass + col2->mass);
		}
		else
		{
			relaxPercentage1 = 1;
			relaxPercentage2 = 0;
		}

		// Calc relax distance
		float relaxDistance1 = overlap * relaxPercentage1;
		float relaxDistance2 = overlap * relaxPercentage2;

		// Relax objects
		col1->game_object->transform.position = col1->game_object->transform.position + (colNormal * relaxDistance1);
		col2->game_object->transform.position = col2->game_object->transform.position - (colNormal * relaxDistance2);


		// Calculate bounce
		float e = 0.5;
		float mass1inverse, mass2inverse = -1;
		float speedDifference = 0.0f;

		mass1inverse = 1 / col1->mass;

		if (col2->mass > 0)
		{
			mass2inverse = 1 / col2->mass;
			speedDifference = col1->rigidbody->velocity.dot(colNormal) - col2->rigidbody->velocity.dot(colNormal);
		}
		else
		{
			mass2inverse = 0;
			speedDifference = col1->rigidbody->velocity.dot(colNormal);
		}

		// Vector from center of mass to point of collision
		Vec2 vecToCol1 = col1->game_object->transform.position - col_point;
		Vec2 vecToCol2 = col2->game_object->transform.position - col_point;
		Vec2 vecToCol1_normal(vecToCol1.y, -vecToCol1.x);
		Vec2 vecToCol2_normal(vecToCol2.y, -vecToCol2.x);

		// Moment of inertia compensated for size
		float rI1 = col1->rigidbody->getMomentOfInertia() * powf(col1->game_object->transform.getScale().x, 2.0f);
		float rI2 = 0;
		if (col2->mass > 0)
			rI2 = col2->rigidbody->getMomentOfInertia() *  powf(col2->game_object->transform.getScale().x, 2.0f);

		// Calculate rotational factor
		float roll1 = powf(vecToCol1_normal.dot(colNormal), 2) / rI1;
		float roll2 = 0;
		if (col2->mass > 0)
			roll2 = powf(vecToCol2_normal.dot(colNormal), 2) / rI2;

		// Calculate impact force
		float impact = -(e + 1) * speedDifference / (mass1inverse + mass2inverse + roll1 + roll2);

		// Apply velocity and angular velocity to objects
		col1->rigidbody->velocity = col1->rigidbody->velocity + (colNormal * impact * mass1inverse);
		col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(colNormal * impact) / rI1);
		if (col2->rigidbody != nullptr && col2->mass > 0)
		{
			col2->rigidbody->velocity = col2->rigidbody->velocity - (colNormal * impact * mass2inverse);
			col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity - (vecToCol2_normal.dot(colNormal * impact) / rI2);
		}

		// End
		return true;
	}

	bool ColliderCircle::collisionCircleCircle(ColliderCircle *col1, ColliderCircle *col2)
	{
		// Get distance squared between objects
		float distance = col1->game_object->transform.position.distanceToSqr(col2->game_object->transform.position);
		float col_distance = (col1->radius * col1->game_object->transform.getScale().x) + (col2->radius * col2->game_object->transform.getScale().x);
		col_distance *= col_distance; // minimum distance for collision squared

									  // If distance is smaller or equal to sum of radius, then we have a collision
		if (col_distance >= distance)
		{
			// Get from col to this vector (normal for rebound velocity change)
			Vec2 colNormal = col1->game_object->transform.position - col2->game_object->transform.position;
			colNormal.normalize();

			float relaxPercentage1, relaxPercentage2;

			// Get relax percentage
			if (col2->mass > 0)
			{
				relaxPercentage1 = col2->mass / (col1->mass + col2->mass);
				relaxPercentage2 = col1->mass / (col1->mass + col2->mass);
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
			col1->game_object->transform.position = col1->game_object->transform.position + (colNormal * relaxDistance1);
			col2->game_object->transform.position = col2->game_object->transform.position - (colNormal * relaxDistance2);


			// Calculate bounce
			float e = 0.5;
			float mass1inverse, mass2inverse = -1;
			float speedDifference = 0.0f;

			mass1inverse = 1 / col1->mass;

			if (col2->mass > 0)
			{
				mass2inverse = 1 / col2->mass;
				speedDifference = col1->rigidbody->velocity.dot(colNormal) - col2->rigidbody->velocity.dot(colNormal);
			}
			else
			{
				mass2inverse = 0;
				speedDifference = col1->rigidbody->velocity.dot(colNormal);
			}

			// Distance to point of collision
			Vec2 vecToCol1 = colNormal * col1->radius;
			Vec2 vecToCol2 = colNormal * -col2->radius;
			Vec2 vecToCol1_normal(vecToCol1.y, -vecToCol1.x);
			Vec2 vecToCol2_normal(vecToCol2.y, -vecToCol2.x);

			// Moment of inertia compensated for size
			float rI1 = col1->rigidbody->getMomentOfInertia() * powf(col1->game_object->transform.getScale().x, 2.0f);
			float rI2 = 0;
			if (col2->mass > 0)
				rI2 = col2->rigidbody->getMomentOfInertia() *  powf(col2->game_object->transform.getScale().x, 2.0f);

			// Calculate rotational factor
			float roll1 = powf(vecToCol1_normal.dot(colNormal), 2) / rI1;
			float roll2 = 0;
			if (col2->mass > 0)
				roll2 = powf(vecToCol2_normal.dot(colNormal), 2) / rI2;

			// Calculate impact force
			float impact = -(e + 1) * speedDifference / (mass1inverse + mass2inverse + roll1 + roll2);

			// Apply velocity and angular velocity to objects
			col1->rigidbody->velocity = col1->rigidbody->velocity + (colNormal * impact * mass1inverse);
			col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(colNormal * impact) / rI1);
			if (col2->rigidbody != nullptr && col2->mass > 0)
			{
				col2->rigidbody->velocity = col2->rigidbody->velocity - (colNormal * impact * mass2inverse);
				col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity - (vecToCol2_normal.dot(colNormal * impact) / rI2);
			}

			// End
			return true;
		}

		return false;
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