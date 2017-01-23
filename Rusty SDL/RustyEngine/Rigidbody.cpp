#include "Rigidbody.h"
#include "Time.h"
#include "GameObject.h"
#include "Physics.h"

namespace RustyEngine
{
	// Actually, it's just simple velocity damping
	void Rigidbody::applyDrag()
	{
		velocity = velocity * (1 - (drag * Time::fixed_delta_t));
		angular_velocity *= 1 - (angular_drag * Time::fixed_delta_t);
	}


	// Calculates composite moment of inertia
	void Rigidbody::calculateMomentOfInertia()
	{
		for (int i = 0; i < colliders.size(); i++)
		{
			// TODO this thing
			//colliders[i]->
		}
	}


	// Constructor
	Rigidbody::Rigidbody()
	{
		//game_object = nullptr;
		drag = 1;
		angular_drag = 1;
		mass = 1;
		velocity.set(0, 0);
		angular_velocity = 0;
		bool use_gravity = true;
	}


	// Destructor
	Rigidbody::~Rigidbody()
	{
		game_object = nullptr;
	}


	// Physics update function called once per physics update (physics loop)
	void Rigidbody::fixedUpdate()
	{
		// Apply gravity first
		velocity = velocity + (Physics::gravity * Time::fixed_delta_t);

		// Calculate new velocity from applied forces
		velocity = velocity + ((force * Time::fixed_delta_t) / mass); // N = kg*m/ s^2
		angular_velocity += (rotational_force * Time::fixed_delta_t) / mass;

		// Then apply drag to it
		applyDrag();

		// Finally, move game object to it's new position
		game_object->transform.position = game_object->transform.position + (velocity * Time::fixed_delta_t);
	}
}