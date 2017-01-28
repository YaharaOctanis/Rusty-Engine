#include "Physics.h"


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
}