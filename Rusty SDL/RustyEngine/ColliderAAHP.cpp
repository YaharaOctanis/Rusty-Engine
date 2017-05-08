#include "ColliderAAHP.h"
#include "GameObject.h"


namespace RustyEngine
{
	ColliderAAHP::ColliderAAHP()
	{
		type = ColliderType::aahp;
		length = 1;
		direction.set(1, 0);
	}

	ColliderAAHP::ColliderAAHP(bool horizontal)
	{
		type = ColliderType::aahp;
		length = 1;

		if (horizontal)
			direction.set(1, 0);
		else
			direction.set(0, 1);
	}


	ColliderAAHP::~ColliderAAHP()
	{
	}


	Vec2 ColliderAAHP::getWorldMin()
	{
		return Vec2(game_object->transform.position.x - (game_object->transform.getScale().x * direction.x * length / 2), 
					game_object->transform.position.y - (game_object->transform.getScale().x * direction.y * length / 2));
	}

	Vec2 ColliderAAHP::getWorldMax()
	{
		return Vec2(game_object->transform.position.x + (game_object->transform.getScale().x * direction.x * length / 2),
					game_object->transform.position.y + (game_object->transform.getScale().x * direction.y * length / 2));
	}
}