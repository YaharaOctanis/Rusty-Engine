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

		center_of_mass = center_of_mass - game_object->transform.position; // Convert center of mass to local space
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
		if (mass <= 0 || area <= 0)
		{
			center_of_mass.set(0, 0);
			return;
		}
		//center_of_mass = (object_1_mass * object_1_position + object_2_mass * object_2_position + ... + object_n_mass * object_n_position) / composite_mass
		
		for (int i = 0; i < colliders.size(); i++)
		{
			colliders[i]->mass = (colliders[i]->area / area) * mass;
			center_of_mass = center_of_mass + (colliders[i]->game_object->transform.position * colliders[i]->mass);
		}

		center_of_mass = center_of_mass / mass;
	}

	void Rigidbody::collisionCheck()
	{
		// Check every collider in the physics world
		for (int i = 0; i < Physics::colliders.size(); i++)
		{
			// Against every collider attached to this rigidbody
			for (int j = 0; j < colliders.size(); j++)
			{
				// If collided, call onCollision solver
				if (colliders[j]->collisionCheck(Physics::colliders[i]))
				{
					continue;
				}
					//onCollision(colliders[j]->game_object, nullptr);
			}
		}
	}


	// Constructor
	Rigidbody::Rigidbody()
	{
		// Public
		drag = 1;
		angular_drag = 1;
		mass = 1;
		velocity.set(0, 0);
		angular_velocity = 0;
		use_gravity = true;

		// Private
		force.set(0, 0);
		torque = 0;
		center_of_mass.set(0, 0);
		moment_of_inertia = 0;
		area = 1;
		updateInertia = true;
	}


	// Destructor
	Rigidbody::~Rigidbody()
	{
		game_object = nullptr;
	}


	// Physics update function called once per physics update (physics loop)
	void Rigidbody::fixedUpdate()
	{
		if (updateInertia)
		{
			calculateMomentOfInertia(); // update moment of inertia when needed
			updateInertia = false;
		}

		// Apply gravity first
		if(use_gravity)
			velocity = velocity + (Physics::gravity * Time::fixed_delta_t);

		// Check for collisions right before applying forces
		collisionCheck();

		// Calculate new velocity from applied forces
		if(mass > 0)
			velocity = velocity + ((force * Time::fixed_delta_t) / mass); // N = kg*m/ s^2

		// w = T * dt / I * scale^2 (will only work if scale is uniform)
		if(moment_of_inertia > 0)
			angular_velocity += (torque * Time::fixed_delta_t) / (moment_of_inertia * powf(game_object->transform.getScale().x, 2.0f));

		// Then apply drag to it
		applyDrag();

		// Check for collisions right before moving
		//collisionCheck();

		// Finally, move game object to it's new position
		game_object->transform.position = game_object->transform.position + (velocity * Time::fixed_delta_t);
		game_object->transform.rotate(angular_velocity * Time::fixed_delta_t * RAD_TO_DEG);

		// And reset force and torque to 0
		force.set(0, 0);
		torque = 0;
	}


	// Called every frame, for each object it collides with, takes game object and list of collision points as a parameter 
	// Resolves collision between two objects
	void Rigidbody::onCollision(GameObject * g_obj, Vec2 * cols)
	{
		//velocity.set(0, 0);
		//angular_velocity = 0;
		//torque = 0;
		//force.set(0, 0);
	}


	// Add force to the center of mass (in N)
	void Rigidbody::addForce(Vec2 force)
	{
		this->force = this->force + force;
	}


	// Add force to object at point in world space (in N)
	void Rigidbody::addForceAtPosition(Vec2 force, Vec2 position)
	{
		// Add force
		this->force = this->force + force;
				
		// vector pointing from position to center of mass
		Vec2 toCoM = game_object->transform.position - position; 

		//sinfi = cross(a,b) / sqrt(a.len^2 * b.len^2)
		float sin_fi = toCoM.cross(force) / sqrtf(toCoM.sqareLength() * force.sqareLength());

		// Add torque
		torque += force.length() * position.distanceTo(center_of_mass) * sin_fi;
	}


	// Adds collider to the composite (this function will also add the collider to game object's list of components)
	void Rigidbody::addCollider(Collider * col)
	{
		col->rigidbody = this;
		colliders.push_back(col);
		Physics::addCollider(col);

		if (game_object != nullptr)
			game_object->addComponent(col);

		updateInertia = true;
	}


	float Rigidbody::getMomentOfInertia()
	{
		return moment_of_inertia;
	}
}