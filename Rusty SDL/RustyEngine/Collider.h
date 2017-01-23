#ifndef RUSTYENGINE_COLLIDER_H
#define RUSTYENGINE_COLLIDER_H

#pragma once

#include "Component.h"

namespace RustyEngine
{
	class Collider : public Component
	{
	private:
		float area;

	public:
		Collider();
		~Collider();
	};
}

#endif // RUSTYENGINE_PHYSICS_H