#include "Crossbow.h"

#include <AudioStreamPlayer.hpp>


using namespace godot;

void Crossbow::_register_methods()
{
	register_method("_init", &Crossbow::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Crossbow::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Crossbow::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Crossbow::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Crossbow::_init()
{
	weaponType_ = static_cast<int64_t>(WeaponType::CROSSBOW);
}

void Crossbow::_ready()
{
	set_physics_process(false);
	set_z_index(3);
	set_z_as_relative(true);
}

void Crossbow::playDrawSound()
{
	if (is_network_master())
	{
		static_cast<AudioStreamPlayer*>(get_node("CrossbowDrawSound"))->play();
	}
}

void Crossbow::playAttackSound()
{
	//if (is_network_master())
	//{
	//	static_cast<AudioStreamPlayer*>(get_node("CrossbowAttackSound"))->play();
	//}
}

void Crossbow::_physics_process(float delta)
{

}

void Crossbow::_process(float delta)
{

}


