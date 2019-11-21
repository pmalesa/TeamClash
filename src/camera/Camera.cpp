#include "Camera.h"

using namespace godot;

void Camera::_register_methods()
{
    register_method("_ready", &Camera::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Camera::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_process", &Camera::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Camera::_init()
{

}

void Camera::_ready()
{

}

void Camera::_process(float delta)
{
    
}