#include "Weapon.h"

using namespace godot;

void Weapon::_register_methods()
{
    register_method("_ready", &Weapon::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Weapon::_init, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Weapon::_init()
{

}

void Weapon::_ready()
{

}
