#include "Component.h"
#include "RustyConstants.h"


namespace RustyEngine
{
	Component::Component()
	{
		active = COMPONENTS_START_ACTIVE;
		game_object = nullptr;
	}

	Component::~Component()
	{
		active = false;
		game_object = nullptr;
	}
}