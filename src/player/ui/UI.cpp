#include "UI.h"

using namespace godot;

void UI::_register_methods()
{
    register_method("_init", &UI::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &UI::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &UI::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}
