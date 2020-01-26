#include "Biome.h"

#include <Sprite.hpp>

using namespace godot;

void Biome::_register_methods()
{
	register_method("_init", &Biome::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &Biome::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Biome::_init()
{

}

void Biome::_ready()
{

}