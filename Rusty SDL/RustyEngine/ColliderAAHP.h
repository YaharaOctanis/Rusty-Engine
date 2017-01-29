#ifndef RUSTYENGINE_COLLIDERAAHP_H
#define RUSTYENGINE_COLLIDERAAHP_H

#pragma once

#define _USE_MATH_DEFINES
#include "ColliderHP.h"

namespace RustyEngine
{
	class ColliderAAHP : public ColliderHP
	{
		friend class Physics;
	public:
		ColliderAAHP();
		ColliderAAHP(bool horizontal);
		~ColliderAAHP();

		Vec2 getWorldMin();
		Vec2 getWorldMax();
	};
}

#endif // RUSTYENGINE_COLLIDERAAHP_H