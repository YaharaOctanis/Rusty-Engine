#include "ColliderRectangle.h"
#include "GameObject.h"
#include <cmath>


namespace RustyEngine
{
	float ColliderRectangle::calculateMomentOfInertia(Vec2 point)
	{
		float I = 0.0f;

		I = (mass/12) * (powf(size.x, 2) + powf(size.y, 2)); // calculate rectangle's inertia
		I += mass * powf(game_object->transform.position.distanceTo(point), 2); // recalculate inertia for cutom hinge point

		return I;
	}

	void ColliderRectangle::calculateArea()
	{
		area = size.x * size.y;
	}


	ColliderRectangle::ColliderRectangle()
	{
	}


	ColliderRectangle::~ColliderRectangle()
	{
	}


	void ColliderRectangle::setSize(Vec2 s)
	{
		size.set(s.x, s.y);
		calculateArea();
	}


	void ColliderRectangle::setSize(float s)
	{
		size.set(s, s);
		calculateArea();
	}


	void ColliderRectangle::setSize(float w, float h)
	{
		size.set(w, h);
		calculateArea();
	}


	const Vec2& ColliderRectangle::getSize()
	{
		return size;
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