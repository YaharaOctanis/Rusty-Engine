#ifndef RUSTYENGINE_COLLIDERAAB_H
#define RUSTYENGINE_COLLIDERAAB_H

#pragma once

#define _USE_MATH_DEFINES
#include "Collider.h"

namespace RustyEngine
{
	class ColliderAAB : public Collider
	{
	public:
		ColliderAAB();
		~ColliderAAB();
	};
}

#endif // RUSTYENGINE_COLLIDERAAB_H