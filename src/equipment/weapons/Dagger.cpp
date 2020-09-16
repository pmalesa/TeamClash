#include "Dagger.h"

#include "../../player/Player.h"

#include <SceneTree.hpp>
#include <AudioStreamPlayer.hpp>
#include <Node2D.hpp>

using namespace godot;

void Dagger::_register_methods()
{
	register_method("_init", &Dagger::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Dagger::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Dagger::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Dagger::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processMeleeAttack", &Dagger::processMeleeAttack, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("playAttackSound", &Dagger::playAttackSound, GODOT_METHOD_RPC_MODE_REMOTESYNC);
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
	if (is_network_master())
	{
		if (get_tree()->is_network_server())
			processMeleeAttack();
		else
			rpc_id(1, "processMeleeAttack");
	}
}

