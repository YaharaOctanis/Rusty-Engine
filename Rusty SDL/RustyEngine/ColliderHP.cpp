#include "ColliderHP.h"
#include "GameObject.h"

namespace RustyEngine
{
	bool ColliderHP::collisionCheck(Collider * col)
	{
		return false;
	}

	ColliderHP::ColliderHP()
	{
		type = ColliderType::hp;
		length = 1;
		direction.set(0, 1);
	}

	ColliderHP::ColliderHP(Vec2 dir)
	{
		type = ColliderType::hp;
		length = 1;
		direction = dir;
	}


	ColliderHP::~ColliderHP()
	{
	}


	Vec2 ColliderHP::getWorldMin()
	{
		Vec2 rotato = direction;
		rotato.rotate(-game_object->transform.getRotation());
		return Vec2(game_object->transform.position.x - (game_object->transform.getScale().x * rotato.x * length / 2),
			game_object->transform.position.y - (game_object->transform.getScale().x * rotato.y * length / 2));
	}

	Vec2 ColliderHP::getWorldMax()
	{
		Vec2 rotato = direction;
		rotato.rotate(-game_object->transform.getRotation());
		return Vec2(game_object->transform.position.x + (game_object->transform.getScale().x * rotato.x * length / 2),
			game_object->transform.position.y + (game_object->transform.getScale().x * rotato.y * length / 2));
	}
}