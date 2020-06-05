#include "Dagger.h"

#include <AudioStreamPlayer.hpp>


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
	damage_ = 14;
	weaponType_ = static_cast<int64_t>(WeaponType::DAGGER);
}

void Dagger::_ready()
{
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
	if (is_network_master())
	{
		static_cast<AudioStreamPlayer*>(get_node("DaggerAttackSound"))->play();
	}
}

void Dagger::_physics_process(float delta)
{

}

void Dagger::_process(float delta)
{

}


