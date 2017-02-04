#include "Physics.h"
#include "GameObject.h"
#include "Rigidbody.h"

// Debug draw stuff
#include <SDL.h>
#include "Game.h"
#include "World.h"
#include <iostream>

namespace RustyEngine
{
	std::vector<Collider*> Physics::colliders;
	Vec2 Physics::gravity(0, -9.807);

	void Physics::addCollider(Collider * col)
	{
		colliders.push_back(col);
	}


	void Physics::clearColliders()
	{
		colliders.clear();
	}


	Vec2* Physics::collisionLineLine(ColliderHP * col1, ColliderHP * col2)
	{
		Vec2 a1 = col1->getWorldMin();
		Vec2 a2 = col1->getWorldMax();
		Vec2 b1 = col2->getWorldMin();
		Vec2 b2 = col2->getWorldMax();

		return collisionLineLine(a1, a2, b1, b2);
	}

	Vec2* Physics::collisionLineLine(Vec2 a1, Vec2 a2, Vec2 b1, Vec2 b2)
	{
		float denominator = ((b2.y - b1.y) * (a2.x - a1.x)) - ((b2.x - b1.x) * (a2.y - a1.y));

		if (denominator == 0)
			return nullptr;


		float ua = (((b2.x - b1.x) * (a1.y - b1.y)) - ((b2.y - b1.y) * (a1.x - b1.x))) / denominator;
		float ub = (((a2.x - a1.x) * (a1.y - b1.y)) - ((a2.y - a1.y) * (a1.x - b1.x))) / denominator;

//		std::cout << a1.x << " " << a1.y << " " << a2.x << " " << a2.y << " " << ua << std::endl;

		if (ua < 0 || ua > 1 || ub < 0 || ub > 1)
			return nullptr;

		return new Vec2(a1 + ((a2 - a1) * ua));
	}

	Vec2* Physics::collisionLineHP(Vec2 a1, Vec2 a2, Vec2 b1, Vec2 b2)
	{
		float denominator = ((b2.y - b1.y) * (a2.x - a1.x)) - ((b2.x - b1.x) * (a2.y - a1.y));

		if (denominator == 0)
			return nullptr;


		float ua = (((b2.x - b1.x) * (a1.y - b1.y)) - ((b2.y - b1.y) * (a1.x - b1.x))) / denominator;
		float ub = (((a2.x - a1.x) * (a1.y - b1.y)) - ((a2.y - a1.y) * (a1.x - b1.x))) / denominator;
		
		if (ua < 0 || ua > 1)
			return nullptr;

		return new Vec2(a1 + ((a2 - a1) * ua));
	}

	// Circle -> Line collider + solver (works)
	bool Physics::collisionCircleHP(ColliderCircle * col1, ColliderHP * col2)
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
		vecToCol1_normal.normalize();
		vecToCol2_normal.normalize();

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
		col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(colNormal * impact) / rI1) * DEG_TO_RAD;
		if (col2->rigidbody != nullptr && col2->mass > 0)
		{
			col2->rigidbody->velocity = col2->rigidbody->velocity - (colNormal * impact * mass2inverse);
			col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity - (vecToCol2_normal.dot(colNormal * impact) / rI2) * DEG_TO_RAD;
		}

		// End
		return true;
	}

	// Circle -> Circle collider + solver (works)
	bool Physics::collisionCircleCircle(ColliderCircle *col1, ColliderCircle *col2)
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
			//vecToCol1_normal.normalize();
			//vecToCol2_normal.normalize();

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
			col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(colNormal * impact) / rI1) * DEG_TO_RAD;
			if (col2->rigidbody != nullptr && col2->mass > 0)
			{
				col2->rigidbody->velocity = col2->rigidbody->velocity - (colNormal * impact * mass2inverse);
				col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity - (vecToCol2_normal.dot(colNormal * impact) / rI2) * DEG_TO_RAD;
			}

			// End
			return true;
		}

		return false;
	}

	// Rectangle -> Line collider + solver (borked at 1 collision point)
	bool Physics::collisionRectangleLine(ColliderRectangle * col1, ColliderHP * col2)
	{
		Vec2 p1;
		Vec2 p2;

		if (col2->getType() == ColliderType::aahp)
		{
			ColliderAAHP* col3 = static_cast<ColliderAAHP*>(col2);
			p1 = col3->getWorldMin();
			p2 = col3->getWorldMax();
		}
		else
		{
			p1 = col2->getWorldMin();
			p2 = col2->getWorldMax();
		}

		return collisionRectangleLine(col1, p1, p2);
	}

	// Rectangle -> Line collider + solver (borked at 1 collision point)
	bool Physics::collisionRectangleLine(ColliderRectangle * col1, Vec2 p1, Vec2 p2)
	{
		// Calculate rectangle's corners
		Vec2 corners[4];
		
		corners[0].set(-(col1->size.x * col1->game_object->transform.getScale().x / 2),
						-(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[1].set((col1->size.x * col1->game_object->transform.getScale().x / 2),
						- (col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[2].set((col1->size.x * col1->game_object->transform.getScale().x / 2),
						(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[3].set(- (col1->size.x * col1->game_object->transform.getScale().x / 2),
						(col1->size.y * col1->game_object->transform.getScale().y / 2));

		corners[0].rotate(-col1->game_object->transform.getRotation());
		corners[1].rotate(-col1->game_object->transform.getRotation());
		corners[2].rotate(-col1->game_object->transform.getRotation());
		corners[3].rotate(-col1->game_object->transform.getRotation());


		corners[0] = corners[0] + col1->game_object->transform.position;
		corners[1] = corners[1] + col1->game_object->transform.position;
		corners[2] = corners[2] + col1->game_object->transform.position;
		corners[3] = corners[3] + col1->game_object->transform.position;
		
		SDL_Color c;
		c.r = 0; c.g = 255; c.b = 0; c.a = 255;
		debugDraw(corners[0], corners[1], c);
		debugDraw(corners[1], corners[2], c);
		debugDraw(corners[2], corners[3], c);
		debugDraw(corners[3], corners[0], c);

		debugDraw(p1, p2, c);

		// Do collision test between rectangle lines and given line
		Vec2* cols[2];
		Vec2 first_dir;
		int col_count = 0;

		for (int i = 0; i < 4; i++)
		{
			cols[col_count] = nullptr;
			if (i + 1 >= 4)
				cols[col_count] = collisionLineHP(corners[i], corners[0], p1, p2);
			else
				cols[col_count] = collisionLineHP(corners[i], corners[i + 1], p1, p2);
			if (cols[col_count] != nullptr)
			{
				if (col_count == 0)
				{
					if (i + 1 >= 4)
						first_dir = corners[0] - corners[i];
					else
						first_dir = corners[i + 1] - corners[i];
				}
				col_count++;
				if (col_count >= 2)
					break;
			}
		}

		if (col_count == 0)
			return false;

		Vec2 p_dir = p2 - p1;
		Vec2 sat_normal;
		Vec2 col_point(cols[0]->x, cols[0]->y);

		if (col_count == 2)
		{
			//sat_normal.set(p_dir.y, -p_dir.x);
			col_point.set((cols[0]->x + cols[1]->x)/2, (cols[0]->y + cols[1]->y)/2);
		}
		/*
		else if (col_count == 1)
			sat_normal.set(col1->game_object->transform.position.x - cols[0]->x, col1->game_object->transform.position.y - cols[0]->y);*/
			//sat_normal.set(first_dir.y, -first_dir.x);

		// Set normal for seperating axis theorem 
		sat_normal.set(p_dir.y, -p_dir.x); // Wall of infinite length 
		sat_normal.normalize();

		// Correct normal, depending on which side od the line we have our object
		if (col_count != 1)
		{
			float flipper_angle = (col1->game_object->transform.position - ((p1 + p2) / 2)).angleBetween(sat_normal) * RAD_TO_DEG;
			if (flipper_angle > 90 && flipper_angle < 270)
				sat_normal = sat_normal * -1;
		}

		c.r = 255;
		debugDraw(sat_normal, sat_normal * 5, c);

		float rect_min = FLT_MAX;
		float rect_max = -FLT_MAX;
		float line_min = FLT_MAX;
		float line_max = -FLT_MAX;

		float temp_proj;
		int max_corner = -1;

		// Get min and max points for both objects (SAT)

		// Rectangle max - min
		for (int i = 0; i < 4; i++)
		{
			temp_proj = corners[i].dot(sat_normal);
			if (temp_proj < rect_min)
				rect_min = temp_proj;
			if (temp_proj > rect_max)
			{
				rect_max = temp_proj;
				max_corner = i;
			}
		}

		// Line max - min
		line_min = p1.dot(sat_normal);
		temp_proj = p2.dot(sat_normal);

		if (temp_proj < line_min)
		{
			line_max = line_min;
			line_min = temp_proj;
		}
		else
			line_max = temp_proj;

		c.g = 0;
		//debugDraw(corners[max_corner], Vec2(corners[max_corner].x * sat_normal.x, corners[max_corner].y *sat_normal.y) * -2, c);

		// Calculate overlap
		float overlap;

		if(col_count > 1)
			overlap = fabsf(rect_min - line_min);
		else
		{
			if (cols[0]->distanceToSqr(p1) < cols[0]->distanceToSqr(p2))
				overlap = cols[0]->distanceTo(p1);
			else
				overlap = cols[0]->distanceTo(p2);
		}


		// sat_normal is seperation axis (collision normal)
		// overlap is minimum separation distance
				
		// Relax rect for overlap
		col1->game_object->transform.position = col1->game_object->transform.position + (sat_normal * overlap);
		
		// Calculate bounce
		float e = 0.5;
		float mass1inverse;
		float speedDifference = 0.0f;
		//Vec2 sat_flipnormal(sat_normal.y, sat_normal.x);
		//sat_normal.set(sat_normal.y, sat_normal.x);

		//col1->rigidbody->velocity.rotate(-col1->rigidbody->velocity.angleBetween(sat_normal));

		mass1inverse = 1 / col1->mass;
		speedDifference = col1->rigidbody->velocity.dot(sat_normal);
		
		// Vector from center of mass to point of collision
		Vec2 vecToCol1 = col1->game_object->transform.position - col_point;
		//Vec2 vecToCol2 = col2->game_object->transform.position - col_point;
		Vec2 vecToCol1_normal(-vecToCol1.y, vecToCol1.x);
		vecToCol1_normal.normalize();
		//Vec2 vecToCol2_normal(vecToCol2.y, -vecToCol2.x);

		// Moment of inertia compensated for size
		float rI1 = col1->rigidbody->getMomentOfInertia() * powf(col1->game_object->transform.getScale().x, 2.0f);

		// Calculate rotational factor
		float roll1 = powf(vecToCol1_normal.dot(sat_normal), 2) / rI1;

		// Calculate impact force
		float impact = -(e + 1) * speedDifference / (mass1inverse + roll1);

		// Apply velocity and angular velocity to objects
		col1->rigidbody->velocity = col1->rigidbody->velocity + (sat_normal * impact * mass1inverse);
		col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(sat_normal * impact) / rI1) * DEG_TO_RAD;

		// End
		return true;
	}

	bool Physics::collisionRectangleCircle(ColliderRectangle * col1, ColliderCircle * col2)
	{
		// Calculate rectangle's corners
		Vec2 corners[4];

		corners[0].set(-(col1->size.x * col1->game_object->transform.getScale().x / 2),
			-(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[1].set((col1->size.x * col1->game_object->transform.getScale().x / 2),
			-(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[2].set((col1->size.x * col1->game_object->transform.getScale().x / 2),
			(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[3].set(-(col1->size.x * col1->game_object->transform.getScale().x / 2),
			(col1->size.y * col1->game_object->transform.getScale().y / 2));

		corners[0].rotate(-col1->game_object->transform.getRotation());
		corners[1].rotate(-col1->game_object->transform.getRotation());
		corners[2].rotate(-col1->game_object->transform.getRotation());
		corners[3].rotate(-col1->game_object->transform.getRotation());


		corners[0] = corners[0] + col1->game_object->transform.position;
		corners[1] = corners[1] + col1->game_object->transform.position;
		corners[2] = corners[2] + col1->game_object->transform.position;
		corners[3] = corners[3] + col1->game_object->transform.position;

		SDL_Color c;
		c.r = 0; c.g = 255; c.b = 0; c.a = 255;
		debugDraw(corners[0], corners[1], c);
		debugDraw(corners[1], corners[2], c);
		debugDraw(corners[2], corners[3], c);
		debugDraw(corners[3], corners[0], c);

		debugDraw(col2->game_object->transform.position, col2->game_object->transform.position + Vec2(col2->radius * col2->game_object->transform.getScale().x, 0), c);

		float min_delta = FLT_MAX;
		Vec2 min_col_point;
		int min_i = -1;
		float min_overlap;
		float min_dist_col2min;
		float min_dist_col2max;

		for (int i = 0; i < 4; i++)
		{

			Vec2 worldMin; 
			Vec2 worldMax;

			worldMin = corners[i];
			if (i + 1 >= 4)
				worldMax = corners[0];
			else
				worldMax = corners[i + 1];

			Vec2 w_dir = worldMax - worldMin;


			Vec2 localMin = worldMin - col2->game_object->transform.position;
			Vec2 localMax = worldMax - col2->game_object->transform.position;
			Vec2 dir = localMax - localMin;

			float line_len_sqr = w_dir.sqareLength();
			float dot2_min_dir = 2 * dir.dot(localMin);

			float delta = powf(dot2_min_dir, 2) - (4 * line_len_sqr * (localMin.sqareLength() - powf(col2->radius * col2->game_object->transform.getScale().x, 2)));
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
					continue;
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
					continue;
				else if (dist_col2min <= line_len_sqr && dist_col2max > line_len_sqr) // Off the line, but col point could be world min
				{
					if (dist_col2min <= col_length)
						col_point = worldMin;
					else
						continue;
				}
				else if (dist_col2min > line_len_sqr && dist_col2max <= line_len_sqr) // Off the line, but col point could be world max
				{
					if (dist_col2max <= col_length)
						col_point = worldMax;
					else
						continue;
				}
				// Else on line normally
			}
			else // No collision
				continue;

			if(col_point.distanceToSqr(col1->game_object->transform.position) < min_col_point.distanceToSqr(col1->game_object->transform.position))
			{
				min_delta = delta;
				min_i = i;
				min_overlap = overlap;
				min_col_point = col_point;
				min_dist_col2min = dist_col2min;
				min_dist_col2max = dist_col2max;
			}
		}
		if (min_i < 0)
			return false;

		// Calculate overlap, when there are two collision points
		if (min_delta > 0)
			min_overlap = (col2->radius * col2->game_object->transform.getScale().x) - col2->game_object->transform.position.distanceTo(min_col_point);
			//min_overlap = (col2->radius * col2->game_object->transform.getScale().x) - corners[min_i].distanceTo(col2->game_object->transform.position);
			//min_overlap = (col2->radius * col2->game_object->transform.getScale().x) - (col1->game_object->transform.position - min_col_point).length();

		// Calculate collision normal
		Vec2 colNormal = (col2->game_object->transform.position - min_col_point) * -1;
		colNormal.normalize();

		c.r = 255;
		debugDraw(colNormal, colNormal * 5, c);

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
		float relaxDistance1 = min_overlap * relaxPercentage1;
		float relaxDistance2 = min_overlap * relaxPercentage2;

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
		Vec2 vecToCol1 = col1->game_object->transform.position - min_col_point;
		Vec2 vecToCol2 = col2->game_object->transform.position - min_col_point;
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
		col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(colNormal * -impact) / rI1) * DEG_TO_RAD;
		if (col2->rigidbody != nullptr && col2->mass > 0)
		{
			col2->rigidbody->velocity = col2->rigidbody->velocity - (colNormal * impact * mass2inverse);
			col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity - (vecToCol2_normal.dot(colNormal * -impact) / rI2) * DEG_TO_RAD;
		}

		// End
		return true;
	}

	void Physics::getCorners(ColliderRectangle *col1, Vec2 corners[])
	{
		corners[0].set(-(col1->size.x * col1->game_object->transform.getScale().x / 2),
			-(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[1].set((col1->size.x * col1->game_object->transform.getScale().x / 2),
			-(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[2].set((col1->size.x * col1->game_object->transform.getScale().x / 2),
			(col1->size.y * col1->game_object->transform.getScale().y / 2));
		corners[3].set(-(col1->size.x * col1->game_object->transform.getScale().x / 2),
			(col1->size.y * col1->game_object->transform.getScale().y / 2));

		corners[0].rotate(-col1->game_object->transform.getRotation());
		corners[1].rotate(-col1->game_object->transform.getRotation());
		corners[2].rotate(-col1->game_object->transform.getRotation());
		corners[3].rotate(-col1->game_object->transform.getRotation());


		corners[0] = corners[0] + col1->game_object->transform.position;
		corners[1] = corners[1] + col1->game_object->transform.position;
		corners[2] = corners[2] + col1->game_object->transform.position;
		corners[3] = corners[3] + col1->game_object->transform.position;
	}

	float sat(Vec2 normals_a[], int n, Vec2 normals_b[], int m, Vec2 points_a[], int a, Vec2 points_b[], int b, Vec2 &col_normal_a, Vec2 &col_normal_b, Vec2 &relax_normal)
	{
		float overlap = 0.0f;

		// Smallest overlap for each set of normals
		float overlap_a = FLT_MAX;
		float overlap_b = FLT_MAX;

		// Max-min projections
		float max_a = -FLT_MAX, max_b = -FLT_MAX;
		float min_a = FLT_MAX, min_b = FLT_MAX;

		// Max-min vertex indexes
		int max_va = -1, max_vb = -1;
		int min_va = -1, min_vb = -1;

		// Smallest overlap normal index
		int normal_index_a = -1;
		int normal_index_b = -1;

		// Temp vars
		float temp_overlap = 0.0f;
		float temp_projection;

		// Get smallest overlap for normals of object A
		for (int i = 0; i < n; i++)
		{
			// Project vertices of object A
			for (int j = 0; j < a; j++)
			{
				temp_projection = points_a[j].dot(normals_a[i]);
				
				// Find max point on normal
				if (temp_projection > max_a)
					max_a = temp_projection;
				if (temp_projection < min_a)
					min_a = temp_projection;
			}

			// Project vertices of object B
			for (int j = 0; j < b; j++)
			{
				temp_projection = points_b[j].dot(normals_a[i]);
				
				// Find min point on normal
				if (temp_projection < min_b)
					min_b = temp_projection;
				if (temp_projection > max_b)
					max_b = temp_projection;
			}

			// Compare to find smaller overlap
			temp_overlap = max_a - min_b;

			if(max_b < min_a  || min_b > max_a)
				return -1;

			if (temp_overlap < overlap_a)
			{
				overlap_a = temp_overlap;
				normal_index_a = i;
			}

			min_a = FLT_MAX; min_b = FLT_MAX;
			max_b = -FLT_MAX; max_a = -FLT_MAX;
		}

		// Get smallest overlap for normals of object B
		for(int i = 0; i < m; i++)
		{
			// Project vertices of object A
			for (int j = 0; j < a; j++)
			{
				temp_projection = points_a[j].dot(normals_b[i]);

				// Find min point on normal
				if (temp_projection < min_a)
					min_a = temp_projection;
				if (temp_projection > max_a)
					max_a = temp_projection;
			}

			// Project vertices of object B
			for (int j = 0; j < b; j++)
			{
				temp_projection = points_b[j].dot(normals_b[i]);

				// Find max point on normal
				if (temp_projection > max_b)
					max_b = temp_projection;
				if (temp_projection < min_b)
					min_b = temp_projection;
			}

			// Compare to find smaller overlap
			temp_overlap = max_b - min_a;

			if (max_a < min_b || min_a > max_b)
				return -1;

			if (temp_overlap < overlap_b)
			{
				overlap_b = temp_overlap;
				normal_index_b = i;
			}

			min_a = FLT_MAX; min_b = FLT_MAX;
			max_b = -FLT_MAX; max_a = -FLT_MAX;
		}

		col_normal_a = normals_a[normal_index_a];
		col_normal_b = normals_b[normal_index_b];

		// Return smallest overlap overall
		if (overlap_a < overlap_b)
		{
			overlap = overlap_a;
			relax_normal = normals_a[normal_index_a];
			//col_normal = normals_a[normal_index_a];
		}
		else
		{
			overlap = overlap_b;
			relax_normal = normals_b[normal_index_b] * -1;
			//col_normal = normals_b[normal_index_b];
		}

		return overlap;
	}

	bool Physics::collisionRectangleRectangle(ColliderRectangle *col1, ColliderRectangle* col2)
	{
		Vec2 corners1[4];
		Vec2 corners2[4];

		getCorners(col1, corners1);
		getCorners(col2, corners2);

		Vec2 normals1[4];
		Vec2 normals2[4];

		for (int i = 0; i < 4; i++)
		{
			normals1[i].set(corners1[i + 1 >= 4 ? 0 : i + 1].y - corners1[i].y, -(corners1[i + 1 >= 4 ? 0 : i + 1].x - corners1[i].x));
			normals2[i].set(corners2[i + 1 >= 4 ? 0 : i + 1].y - corners2[i].y, -(corners2[i + 1 >= 4 ? 0 : i + 1].x - corners2[i].x));
			normals1[i].normalize();
			normals2[i].normalize();
		}

		Vec2 col_normal;
		Vec2 col_normal1;
		Vec2 col_normal2;
		Vec2 col_point;
		Vec2 relax_normal;

		// Preform SAT algorithm (returns overlap, collision normal and collision point) - returns -1 when there is no collision
		float overlap = sat(normals1, 4, normals2, 4, corners1, 4, corners2, 4, col_normal1, col_normal2, relax_normal);

		// If overlap is less than 0, then we have no collision 
		if (overlap <= 0)
			return false;

		Vec2* col_tmp = nullptr;
		Vec2 col_avg(0, 0);
		int col_count = 0;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				col_tmp = collisionLineLine(corners1[i], corners1[i + 1 >= 4 ? 0 : i + 1], corners2[j], corners2[j + 1 >= 4 ? 0 : j + 1]);

				if (col_tmp != nullptr)
				{
					col_avg.set(col_avg.x + col_tmp->x, col_avg.y + col_tmp->y);
					col_count++;
				}
			}
		}

		// If collider is inside another collider, ignore collision handling
		if (col_count == 0)
			return false;
		
		// Calculate average collision point for when there are multiple contact points
		col_avg.set(col_avg.x / col_count, col_avg.y / col_count);
		col_point = col_avg;

		/* DEBUG DRAW */
		SDL_Color dc; dc.r = 0; dc.g = 255; dc.b = 0; dc.a = 255;
		debugDraw(col_point, col_point + (relax_normal * 5), dc);
		//debugDraw(col_point, col_point + (col_normal1 * 5), dc);
		//debugDraw(col_point, col_point + (col_normal2 * 5), dc);
		/* DEBUG DRAW */

		// Relax rects for overlap
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

		// Calculate relax distance
		float relaxDistance1 = overlap * relaxPercentage1;
		float relaxDistance2 = -overlap * relaxPercentage2;

		// Relax objects in the direction of one collision normal (must clear object in the same frame, for accurate forces)
		col1->game_object->transform.position = col1->game_object->transform.position + (relax_normal * relaxDistance1);
		col2->game_object->transform.position = col2->game_object->transform.position - (relax_normal * relaxDistance2);
		//col1->game_object->transform.position = col1->game_object->transform.position + (col_normal2 * relaxDistance1);
		//col2->game_object->transform.position = col2->game_object->transform.position - (col_normal2 * relaxDistance2);

		col_normal = relax_normal;
		Vec2 v_pa = col1->rigidbody->velocity + Vec2(col1->game_object->transform.position.y - col_point.y, col1->game_object->transform.position.x - col_point.x) * col1->rigidbody->angular_velocity;
		Vec2 v_pb;
		
		if(col2->mass > 0)
			v_pb = col2->rigidbody->velocity + Vec2(col2->game_object->transform.position.y - col_point.y, col2->game_object->transform.position.x - col_point.x) * col2->rigidbody->angular_velocity;


		/*
		
		hitrost tocke P = hitrost objekta + rotacija objekta * razdalja_med_tocko_p_in_cetrom_objekta_(rotacije)

		Vpa = Va + Wa * Vec2(y0 - yp, x0 - xp)
		
		*/

		// Calculate bounce
		float e = 0.8;
		float mass1inverse, mass2inverse = -1;
		Vec2 speedDifference;
		//float speedDifference1 = 0.0f;
		//float speedDifference2 = 0.0f;

		mass1inverse = 1 / col1->mass;
		speedDifference = v_pa - v_pb;

		if (col2->mass > 0)
		{
			mass2inverse = 1 / col2->mass;
		//	speedDifference = col1->rigidbody->velocity.dot(col_normal) - col2->rigidbody->velocity.dot(col_normal);
			//speedDifference1 = col1->rigidbody->velocity.dot(col_normal1) - col2->rigidbody->velocity.dot(col_normal1);
			//speedDifference2 = col1->rigidbody->velocity.dot(col_normal2) - col2->rigidbody->velocity.dot(col_normal2);
		}
		else
		{
			mass2inverse = 0;
			//speedDifference1 = col1->rigidbody->velocity.dot(col_normal1);
			//speedDifference2 = col1->rigidbody->velocity.dot(col_normal2);
		}

		// Distance to point of collision
		Vec2 vecToCol1 = col1->game_object->transform.position - col_point;
		Vec2 vecToCol2 = col2->game_object->transform.position - col_point;
		Vec2 vecToCol1_normal(-vecToCol1.y, vecToCol1.x);
		Vec2 vecToCol2_normal(-vecToCol2.y, vecToCol2.x);
		vecToCol1_normal.normalize();
		vecToCol2_normal.normalize();

		// Moment of inertia compensated for size
		float rI1 = col1->rigidbody->getMomentOfInertia() * powf(col1->game_object->transform.getScale().x, 2.0f);
		float rI2 = 0;
		if (col2->mass > 0)
			rI2 = col2->rigidbody->getMomentOfInertia() *  powf(col2->game_object->transform.getScale().x, 2.0f);

		// Calculate rotational factor
		float roll1 = powf(vecToCol1_normal.dot(col_normal), 2) / rI1;
		float roll2 = 0;
		if(col2->mass > 0)
			roll2 = powf(vecToCol2_normal.dot(col_normal), 2) / rI2;
		
		/*
		float roll1a = powf(vecToCol1_normal.dot(col_normal1), 2) / rI1;
		float roll2a = 0;
		if (col2->mass > 0)
			roll2a = powf(vecToCol2_normal.dot(col_normal1), 2) / rI2;

		float roll1b = powf(vecToCol1_normal.dot(col_normal2), 2) / rI1;
		float roll2b = 0;
		if (col2->mass > 0)
			roll2b = powf(vecToCol2_normal.dot(col_normal2), 2) / rI2;
			*/

		// Calculate impact force
		//float impact = ((speedDifference * (-e - 1))) / (col_normal.dot(col_normal) * (mass1inverse + mass2inverse));
		float impact = (speedDifference * (-e - 1)).dot(col_normal) / (col_normal.dot(col_normal) * (mass1inverse + mass2inverse));// + roll1 + roll2);
		//float impact1 = (speedDifference * (-e - 1)).dot(col_normal1) / (col_normal1.dot(col_normal1) * (mass1inverse + mass2inverse));// +roll1a + roll2a);
		//float impact2 = (speedDifference * (-e - 1)).dot(col_normal2) / (col_normal2.dot(col_normal2) * (mass1inverse + mass2inverse));// +roll1b + roll2b);

		col1->rigidbody->velocity = col1->rigidbody->velocity + (col_normal * (impact * mass1inverse));
		//col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(col_normal * impact) / rI1);// *DEG_TO_RAD;

		if (col2->rigidbody != nullptr && col2->mass > 0)
		{
			col2->rigidbody->velocity = col2->rigidbody->velocity + (col_normal * (-impact * mass2inverse));
			//col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity + (vecToCol2_normal.dot(col_normal * -impact) / rI2);// *DEG_TO_RAD;
		}

		//float impact1 = -(e + 1) * speedDifference1 / (mass1inverse + mass2inverse + roll1a + roll2a);
		//float impact2 = -(e + 1) * speedDifference2 / (mass1inverse + mass2inverse + roll1b + roll2b);

		// Apply velocity and angular velocity to objects
		//col1->rigidbody->velocity = col1->rigidbody->velocity + (col_normal1 * impact1 * mass1inverse);
		//col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(col_normal1 * impact1) / rI1) * DEG_TO_RAD;
		//if (col2->rigidbody != nullptr && col2->mass > 0)
		//{
		//	col2->rigidbody->velocity = col2->rigidbody->velocity - (col_normal2 * impact2 * mass2inverse);
		//	col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity - (vecToCol2_normal.dot(col_normal2 * impact2) / rI2) * DEG_TO_RAD;
		//}

		// End
		return true;
	}

	bool Physics::collisionRectangleRectangle_old(ColliderRectangle * col1, ColliderRectangle * col2)
	{
		// Calculate rectangle's corners
		Vec2 corners[4];
		Vec2 corners2[4];

		getCorners(col1, corners);
		getCorners(col2, corners2);

		SDL_Color c;
		c.r = 0; c.g = 255; c.b = 0; c.a = 255;
		debugDraw(corners[0], corners[1], c);
		debugDraw(corners[1], corners[2], c);
		debugDraw(corners[2], corners[3], c);
		debugDraw(corners[3], corners[0], c);
		c.r = 0; c.g = 255; c.b = 255; c.a = 255;
		debugDraw(corners2[0], corners2[1], c);
		debugDraw(corners2[1], corners2[2], c);
		debugDraw(corners2[2], corners2[3], c);
		debugDraw(corners2[3], corners2[0], c);


		// Do collision test between rectangle lines
		Vec2* cols[8];
		Vec2 first_dir;
		int col_count = 0;

		Vec2* max_x_col = nullptr;
		Vec2* max_y_col = nullptr;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				cols[col_count] = collisionLineLine(corners[i], corners[i + 1 >= 4 ? 0 : i + 1], corners2[j], corners2[j + 1 >= 4 ? 0 : j + 1]);

				if (cols[col_count] != nullptr)
				{
					if (col_count == 0)
						first_dir = corners[i+1 >= 4 ? 0 : i+1] - corners[i];
					col_count++;

					if (col_count >= 8)
						break;
				}
			}
			if (col_count >= 8)
				break;
		}

		if (col_count == 0)
			return false;

		float dx_min = FLT_MAX;
		float dy_min = FLT_MAX;
		max_x_col = cols[0];
		max_y_col = cols[0];

		// Find collision points closest to the second rectangle
		for (int i = 0; i < col_count; i++)
		{
			float dist_x = fabsf(cols[i]->x - col2->game_object->transform.position.x);
			float dist_y = fabsf(cols[i]->y - col2->game_object->transform.position.y);

			if (dist_x < dx_min)
			{
				dx_min = dist_x;
				max_x_col = cols[i];
			}
			if (dist_y < dy_min)
			{
				dy_min = dist_y;
				max_y_col = cols[i];
			}
		}

		// Find collision point average
		Vec2 col_point(0, 0);
		for (int i = 0; i < col_count; i++)
		{
			col_point.x += cols[i]->x;
			col_point.y += cols[i]->y;
		}
		col_point.x /= col_count;
		col_point.y /= col_count;

		// Draw normals from center of each object, to collision average
		Vec2 sat_normal1 = col1->game_object->transform.position - col_point;
		Vec2 sat_normal2 = col2->game_object->transform.position - col_point;
		
		sat_normal1.normalize();
		sat_normal2.normalize();

		// Then draw normal between those two points
		//Vec2 sat_normal(max_x_col->x - max_y_col->x, max_x_col->y - max_y_col->y);
		/*
		Vec2 p_dir = p2 - p1;
		Vec2 sat_normal;

		// Set normal for seperating axis theorem 
		if (col_count == 2)
			sat_normal.set(p_dir.y, -p_dir.x);
		else if (col_count == 1)
			sat_normal.set(first_dir.y, -first_dir.x);*/
		//sat_normal.normalize();

		/*
		float flipper_angle = (col1->game_object->transform.position - col2->game_object->transform.position).angleBetween(sat_normal) * RAD_TO_DEG;
		if (flipper_angle > 90 && flipper_angle < 270)
			sat_normal = sat_normal * -1;
			*/
		c.r = 255;
		debugDraw(Vec2(0, 0), sat_normal1 * 5, c);
		debugDraw(Vec2(0, 0), sat_normal2 * 5, c);
		debugDraw(col_point, col_point + Vec2(0, 5), c);

		float rect1_min1 = FLT_MAX;
		float rect2_min1 = FLT_MAX;
		float rect1_min2 = FLT_MAX;
		float rect2_min2 = FLT_MAX;
		float rect1_max1 = -FLT_MAX;
		float rect2_max1 = -FLT_MAX;
		float rect1_max2 = -FLT_MAX;
		float rect2_max2 = -FLT_MAX;

		/*
		float rect_min = FLT_MAX;
		float rect_max = -FLT_MAX;
		float rect2_min = FLT_MAX;
		float rect2_max = -FLT_MAX;
		*/
		float temp_proj;
		int max_corner = -1;
		int max_corner2 = -1;

		// Get min and max points for both objects
		for (int i = 0; i < 4; i++)
		{
			// Rect 1 points projected onto sat normal 1 (rect 1 -> col. avg.)
			temp_proj = corners[i].dot(sat_normal1);
			if (temp_proj < rect1_min1)
				rect1_min1 = temp_proj;
			if (temp_proj > rect1_max1)
			{
				rect1_max1 = temp_proj;
				max_corner = i;
			}

			// Rect 2 points projected onto sat normal 1 (rect 1 -> col. avg.)
			temp_proj = corners2[i].dot(sat_normal1);
			if (temp_proj < rect2_min1)
				rect2_min1 = temp_proj;
			if (temp_proj > rect2_max1)
			{
				rect2_max1 = temp_proj;
				max_corner = i;
			}

			// Rect 1 points projected onto sat normal 2 (rect 2 -> col. avg.) 
			temp_proj = corners[i].dot(sat_normal2);
			if (temp_proj < rect1_min2)
				rect1_min2 = temp_proj;
			if (temp_proj > rect1_max2)
			{
				rect1_max2 = temp_proj;
				max_corner = i;
			}

			// Rect 2 points projected onto sat normal 2 (rect 2 -> col. avg.)
			temp_proj = corners2[i].dot(sat_normal2);
			if (temp_proj < rect2_min2)
				rect2_min2 = temp_proj;
			if (temp_proj > rect2_max2)
			{
				rect2_max2 = temp_proj;
				max_corner = i;
			}
		}
		
		c.g = 0;
		//debugDraw(corners[max_corner], Vec2(corners[max_corner].x * sat_normal.x, corners[max_corner].y *sat_normal.y) * -2, c);

		float col_proj1 = col_point.dot(sat_normal1);
		float col_proj2 = col_point.dot(sat_normal2);

		float overlap1 = fabsf(rect1_min1 - col_proj1);
		float overlap2 = fabsf(rect2_min2 - col_proj2);

		float overlap;
		Vec2 sat_normal;



		if (overlap1 < overlap2)
		{
			overlap = overlap1;
			sat_normal = sat_normal1;
		}
		else
		{
			overlap = overlap2;
			sat_normal = sat_normal2 * -1; // Flip normal if using second object
		}

		// sat_normal1 is seperation axis (collision normal) for col1
		// sat_normal2 is seperation axis (collision normal) for col2
		// overlaps are minimum separation distances for both object, to clear each other

		// Relax rects for overlap
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
		col1->game_object->transform.position = col1->game_object->transform.position + (sat_normal * relaxDistance1);
		col2->game_object->transform.position = col2->game_object->transform.position - (sat_normal * relaxDistance2);

		// Calculate bounce
		float e = 0.5;
		float mass1inverse, mass2inverse = -1;
		float speedDifference = 0.0f;
		//sat_normal.set(sat_normal.y, sat_normal.x);

		mass1inverse = 1 / col1->mass;

		if (col2->mass > 0)
		{
			mass2inverse = 1 / col2->mass;
			speedDifference = col1->rigidbody->velocity.dot(sat_normal) - col2->rigidbody->velocity.dot(sat_normal);
		}
		else
		{
			mass2inverse = 0;
			speedDifference = col1->rigidbody->velocity.dot(sat_normal);
		}

		// Distance to point of collision
		Vec2 vecToCol1 = col1->game_object->transform.position - col_point;//corners[max_corner];
		Vec2 vecToCol2 = col2->game_object->transform.position - col_point;//corners2[max_corner2];
		Vec2 vecToCol1_normal(-vecToCol1.y, vecToCol1.x);
		Vec2 vecToCol2_normal(-vecToCol2.y, vecToCol2.x);
		vecToCol1_normal.normalize();
		vecToCol2_normal.normalize();

		// Moment of inertia compensated for size
		float rI1 = col1->rigidbody->getMomentOfInertia() * powf(col1->game_object->transform.getScale().x, 2.0f);
		float rI2 = 0;
		if (col2->mass > 0)
			rI2 = col2->rigidbody->getMomentOfInertia() *  powf(col2->game_object->transform.getScale().x, 2.0f);

		// Calculate rotational factor
		float roll1 = powf(vecToCol1_normal.dot(sat_normal), 2) / rI1;
		float roll2 = 0;
		if (col2->mass > 0)
			roll2 = powf(vecToCol2_normal.dot(sat_normal), 2) / rI2;

		// Calculate impact force
		float impact = -(e + 1) * speedDifference / (mass1inverse + mass2inverse + roll1 + roll2);

		// Apply velocity and angular velocity to objects
		col1->rigidbody->velocity = col1->rigidbody->velocity + (sat_normal * impact * mass1inverse);
		col1->rigidbody->angular_velocity = col1->rigidbody->angular_velocity + (vecToCol1_normal.dot(sat_normal * impact) / rI1) * DEG_TO_RAD;
		if (col2->rigidbody != nullptr && col2->mass > 0)
		{
			col2->rigidbody->velocity = col2->rigidbody->velocity - (sat_normal * impact * mass2inverse);
			col2->rigidbody->angular_velocity = col2->rigidbody->angular_velocity - (vecToCol2_normal.dot(sat_normal * impact) / rI2) * DEG_TO_RAD;
		}

		// End
		return true;
	}
	

	void Physics::debugDraw(Vec2 start, Vec2 end, SDL_Color c)
	{
		return;
		int w, h; // render width and height
		SDL_RenderGetLogicalSize(Game::world.main_renderer, &w, &h);		// get render target size

		if (w == 0 || h == 0)		// if no render target size
			SDL_GetRendererOutputSize(Game::world.main_renderer, &w, &h);	// get screen size instead

		// Calculate position of sprite on screen (can be off-screen)
		Vec2 start_screen;
		start_screen.x = roundf((start.x * 32) + (w / 2.0f) - (Game::world.active_camera->transform.position.x * 32));
		start_screen.y = roundf((-start.y * 32) + (h / 2.0f) + (Game::world.active_camera->transform.position.y * 32));
		Vec2 end_screen;
		end_screen.x = roundf((end.x * 32) + (w / 2.0f) - (Game::world.active_camera->transform.position.x * 32));
		end_screen.y = roundf((-end.y * 32) + (h / 2.0f) + (Game::world.active_camera->transform.position.y * 32));

		// Render sprite on screen with given rotation (if any)
		SDL_SetRenderDrawColor(Game::world.main_renderer, c.r, c.g, c.b, c.a);
		SDL_RenderDrawLine(Game::world.main_renderer, start_screen.x, start_screen.y, end_screen.x, end_screen.y);
		SDL_SetRenderDrawColor(Game::world.main_renderer, 0, 0, 0, 255);
	}

}