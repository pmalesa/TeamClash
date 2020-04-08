#include "HealthBar.h"

using namespace godot;

void HealthBar::_register_methods()
{
    register_method("_ready", &HealthBar::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &HealthBar::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setValue", &HealthBar::setValue, GODOT_METHOD_RPC_MODE_DISABLED);
}

void HealthBar::_init()
{

}

void HealthBar::_ready()
{

}

void HealthBar::setValue(int64_t newValue)
{
	set_value(newValue);
	if (newValue > 0.9 * get_max())
		set_tint_progress(Color(0, 1, 0, 1));
	else if (newValue > 0.7 * get_max())
		set_tint_progress(Color(0.56, 0.93, 0.56, 1));
	else if (newValue > 0.4 * get_max())
		set_tint_progress(Color(1, 1, 0, 1));
	else if (newValue > 0.2 * get_max())
		set_tint_progress(Color(1, 0.65, 0, 1));
	else
		set_tint_progress(Color(1, 0, 0, 1));
}