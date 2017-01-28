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
		rigidbody = nullptr;
	}


	Collider::~Collider()
	{
	}
}