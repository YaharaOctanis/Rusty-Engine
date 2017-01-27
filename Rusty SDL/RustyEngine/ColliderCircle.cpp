#include "ColliderCircle.h"
#include "GameObject.h"
#include <cmath>

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
		setRadius(1.0f);
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