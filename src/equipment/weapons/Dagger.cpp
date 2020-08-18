#include "Dagger.h"

#include "../../player/Player.h"

#include <AudioStreamPlayer.hpp>
#include <Node2D.hpp>

using namespace godot;

void Dagger::_register_methods()
{
	register_method("_init", &Dagger::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Dagger::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Dagger::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Dagger::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Dagger::_init()
{
	setDamage(14);
	setWeaponType(WeaponType::DAGGER);
	setWeaponState(WeaponState::IDLE);
}

void Dagger::_ready()
{
	setOwner(get_node("/root/Game")->call("getPlayer", int64_t(Variant(get_parent()->get_parent()->get_name()))));
	set_physics_process(false);
	set_z_index(3);
	set_z_as_relative(true);
}

void Dagger::playDrawSound()
{
	if (is_network_master())
	{
		static_cast<AudioStreamPlayer*>(get_node("DaggerDrawSound"))->play();
	}
}

void Dagger::playAttackSound()
{
	static_cast<AudioStreamPlayer*>(get_node("DaggerAttackSound"))->play();
}

void Dagger::_physics_process(float delta)
{

}

void Dagger::_process(float delta)
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
				if (attackedPlayer->get_name() != getOwner()->get_name() && !(alreadyAttackedPlayers_.has(attackedPlayer)) && attackedPlayer->is_network_master())
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


