#include "Axe.h"

#include "../../player/Player.h"

#include <AudioStreamPlayer.hpp>
#include <Node2D.hpp>

using namespace godot;

void Axe::_register_methods()
{
	register_method("_init", &Axe::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Axe::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Axe::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Axe::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Axe::_init()
{
	setDamage(34);
	setWeaponType(WeaponType::AXE);
	setWeaponState(WeaponState::IDLE);
}

void Axe::_ready()
{
	setOwner(get_node("/root/Game")->call("getPlayer", int64_t(Variant(get_parent()->get_parent()->get_name()))));
	set_physics_process(false);
	set_z_index(3);
	set_z_as_relative(true);
}

void Axe::playDrawSound()
{
	if (is_network_master())
	{
		static_cast<AudioStreamPlayer*>(get_node("AxeDrawSound"))->play();
	}
}

void Axe::playAttackSound()
{
	static_cast<AudioStreamPlayer*>(get_node("AxeAttackSound"))->play();
}

void Axe::_physics_process(float delta)
{

}

void Axe::_process(float delta)
{
	if (getWeaponState() == WeaponState::ATTACKING)
	{
		playAttackSound();
		set_physics_process(true);
		Array overlapingBodies = get_overlapping_bodies();
		if (overlapingBodies.empty())
			return;

		for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
		{
			Node* overlappedNode = static_cast<Node*>(overlapingBodies[i]);
			if (!overlappedNode->is_in_group("Player"))
				continue;
			else
			{
				Player* attackedPlayer = static_cast<Player*>(overlapingBodies[i]);
				if (attackedPlayer->get_name() != getOwner()->get_name() && !(alreadyAttackedPlayers_.has(attackedPlayer)))
				{
					attackedPlayer->inflictDamage(getDamage());
					if (attackedPlayer->getHealthPoints() > 0)
						attackedPlayer->applyThrowback(attackedPlayer->get_position() - getOwnerPosition());
					alreadyAttackedPlayers_.push_front(attackedPlayer);
				}
			}
		}
		set_physics_process(false);
	}
	else if (!alreadyAttackedPlayers_.empty())
		alreadyAttackedPlayers_.clear();
}


