#include "Weapon.h"

using namespace godot;

void Weapon::_register_methods()
{
	register_method("_init", &Weapon::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Weapon::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Weapon::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Weapon::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}
