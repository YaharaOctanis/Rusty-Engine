#include "Physics.h"
#include "GameObject.h"
#include "Rigidbody.h"

// Debug draw stuff
#include <SDL.h>
#include "Game.h"
#include "World.h"

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

		if (ua < 0 || ua > 1 || ub < 0 || ub > 1)
			return nullptr;

		return &(a1 + ((a2 - a1) * ua));
	}

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
			if (i + 1 >= 4)
				cols[col_count] = collisionLineLine(corners[i], corners[0], p1, p2);
			else
				cols[col_count] = collisionLineLine(corners[i], corners[i + 1], p1, p2);
			if (cols[col_count] != nullptr)
			{
				if (col_count == 0)
				{
					if (i + 1 >= 4)
						first_dir = corners[0] - corners[i];
					else
						first_dir = corners[i + 1] - corners[i];
				}
				if (col_count < 2)
					col_count++;
				else
					break;
			}
		}

		if (col_count == 0)
			return false;

		Vec2 p_dir = p2 - p1;
		Vec2 sat_normal;

		// Set normal for seperating axis theorem 
		if (col_count == 2)
			sat_normal.set(p_dir.y, -p_dir.x);
		else if (col_count == 1)
			sat_normal.set(first_dir.y, -first_dir.x);
		sat_normal.normalize();

		float flipper_angle = (col1->game_object->transform.position - ((p1 + p2) / 2)).angleBetween(sat_normal) * RAD_TO_DEG;
		if (flipper_angle > 90 && flipper_angle < 270)
			sat_normal = sat_normal * -1;

		c.r = 255;
		debugDraw(sat_normal, sat_normal * 5, c);

		float rect_min = FLT_MAX;
		float rect_max = -FLT_MAX;
		float line_min = FLT_MAX;
		float line_max = -FLT_MAX;

		float temp_proj;
		int max_corner = -1;

		// Get min and max points for both objects
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

		float overlap = rect_min - line_min;

		// Containment check (if line is inside rectangle)
		if (line_min > rect_min && line_max < rect_max)
		{
			float mins = fabsf(line_min - rect_min);
			float maxs = fabsf(line_max - rect_max);

			if (mins < maxs)
			{
				//overlap += mins;
			}
			else
			{
				// flip axis in one of if, to flip seperating axis, when needed
				//sat_normal.set(-sat_normal.x, -sat_normal.y);
				//overlap += maxs;
			}
		}

		// sat_normal is seperation axis (collision normal)
		// overlap is minimum separation distance
				
		// Relax rect for overlap
		col1->game_object->transform.position = col1->game_object->transform.position + (sat_normal * overlap);
		
		// Calculate bounce
		float e = 0.5;
		float mass1inverse;
		float speedDifference = 0.0f;
		sat_normal.set(sat_normal.y, sat_normal.x);

		//col1->rigidbody->velocity.rotate(-col1->rigidbody->velocity.angleBetween(sat_normal));

		mass1inverse = 1 / col1->mass;
		speedDifference = col1->rigidbody->velocity.dot(sat_normal);
		
		// Vector from center of mass to point of collision
		Vec2 vecToCol1 = col1->game_object->transform.position - corners[max_corner];
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


	void Physics::debugDraw(Vec2 start, Vec2 end, SDL_Color c)
	{
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