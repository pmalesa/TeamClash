#include "Axe.h"

#include <AudioStreamPlayer.hpp>


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
	damage_ = 34;
	weaponType_ = static_cast<int64_t>(WeaponType::AXE);
}

void Axe::_ready()
{
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
	if (is_network_master())
	{
		static_cast<AudioStreamPlayer*>(get_node("AxeAttackSound"))->play();
	}
}

void Axe::_physics_process(float delta)
{

}

void Axe::_process(float delta)
{

}


