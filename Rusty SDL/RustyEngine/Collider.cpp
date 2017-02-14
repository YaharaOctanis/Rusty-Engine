#include "Collider.h"


namespace RustyEngine
{
	const ColliderType Collider::getType()
	{
		return type;
	}

	Collider::Collider()
	{
		area = 0.0f;
		mass = 0.0f;
		type = ColliderType::none;
		collided = false;
		last_col_normal.set(0, 0);
		rigidbody = nullptr;
		isTrigger = false;
	}


	Collider::~Collider()
	{
	}
}