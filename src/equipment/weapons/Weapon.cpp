#include "Weapon.h"

#include "../../player/Player.h"
#include "../../npc/monsters/Monster.h"

#include <SceneTree.hpp>

using namespace godot;

void Weapon::_register_methods()
{
	register_method("_init", &Weapon::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Weapon::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Weapon::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Weapon::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Weapon::processMeleeAttack()
{
	if (!get_tree()->is_network_server())
		return;

	if (getWeaponState() == WeaponState::ATTACKING)
	{
		rpc("playAttackSound");
		set_physics_process(true);
		Array overlapingBodies = get_overlapping_bodies();
		if (overlapingBodies.empty())
			return;

		for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
		{
			Node* overlappedNode = static_cast<Node*>(overlapingBodies[i]);
			if (overlappedNode->is_in_group("Player"))
			{
				Player* attackedPlayer = static_cast<Player*>(overlapingBodies[i]);
				if (attackedPlayer->get_name() != getOwner()->get_name() && !(alreadyAttackedNodes_.has(attackedPlayer)))
				{
					attackedPlayer->inflictDamage(getDamage(), owner_);
					if (attackedPlayer->getHealthPoints() > 0)
						attackedPlayer->applyThrowback(attackedPlayer->get_position() - getOwnerPosition());
					alreadyAttackedNodes_.push_front(attackedPlayer);
				}
			}
			else if (overlappedNode->is_in_group("Monster"))
			{
				Monster* attackedMonster = static_cast<Monster*>(overlapingBodies[i]);
				if (!(alreadyAttackedNodes_.has(attackedMonster)))
				{
					attackedMonster->inflictDamage(getDamage(), owner_);
					if (attackedMonster->getHealthPoints() > 0/* && !attackedMonster->is_in_group("Elite")*/)
						attackedMonster->applyThrowback(attackedMonster->get_position() - getOwnerPosition());
					alreadyAttackedNodes_.push_front(attackedMonster);
				}
			}
			else
				continue;
		}
		set_physics_process(false);
	}
	else if (!alreadyAttackedNodes_.empty())
		alreadyAttackedNodes_.clear();
}
