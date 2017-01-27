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
		calculateArea(); // n time operation, which returns composite surface area (not accounting for overlap)
		calculateCenterOfMass(); // calculate center of mass and collider mass
		
		float col_mass;
		
		for (int i = 0; i < colliders.size(); i++)
			moment_of_inertia += colliders[i]->calculateMomentOfInertia(center_of_mass);
	}


	// Calculates composite area
	void Rigidbody::calculateArea()
	{
		area = 0.0f;
		for (int i = 0; i < colliders.size(); i++)
			area += colliders[i]->area;
	}

	
	// Calculate mathematical center of mass (and mass of each collider)
	void Rigidbody::calculateCenterOfMass()
	{
		//center_of_mass = (object_1_mass * object_1_position + object_2_mass * object_2_position + ... + object_n_mass * object_n_position) / composite_mass
		
		for (int i = 0; i < colliders.size(); i++)
		{
			colliders[i]->mass = (colliders[i]->area / area) * mass;
			center_of_mass = center_of_mass + (colliders[i]->game_object->transform.position * colliders[i]->mass);
		}

		center_of_mass = center_of_mass / mass;
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
		angular_velocity += (torque * Time::fixed_delta_t) / mass;

		// Then apply drag to it
		applyDrag();

		// Finally, move game object to it's new position
		game_object->transform.position = game_object->transform.position + (velocity * Time::fixed_delta_t);
	}


	// Add force to the center of mass (in N)
	void Rigidbody::addForce(Vec2 force)
	{
		this->force = this->force + force;
	}


	// Add force to object at point in space
	void Rigidbody::addForceAtPosition(Vec2 force, Vec2 position)
	{
		this->force = this->force + force;

		// Fix center of mass, to be local after all original calulations are done
		//torque = force.length() * position.distanceTo(center_of_mass)
	}
}