#include "Axe.h"

#include "../../player/Player.h"

#include <SceneTree.hpp>
#include <AudioStreamPlayer.hpp>
#include <Node2D.hpp>

using namespace godot;

void Axe::_register_methods()
{
	register_method("_init", &Axe::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Axe::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Axe::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Axe::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processMeleeAttack", &Axe::processMeleeAttack, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("playAttackSound", &Axe::playAttackSound, GODOT_METHOD_RPC_MODE_REMOTESYNC);
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
	if (is_network_master())
	{
		if (get_tree()->is_network_server())
			processMeleeAttack();
		else
			rpc_id(1, "processMeleeAttack");
	}
}

