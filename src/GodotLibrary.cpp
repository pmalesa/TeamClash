#include "scenes/MainMenu.h"
#include "scenes/CreateGameScene.h"
#include "scenes/JoinGameScene.h"
#include "scenes/Game.h"
#include "player/Player.h"
#include "camera/Camera.h"

#include "network/Network.h"

#include <gdnative_api_struct.gen.h>


extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* o)
{
	godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* o)
{
	godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* handle)
{
	godot::Godot::nativescript_init(handle);
	godot::register_class<godot::MainMenu>();
	godot::register_class<godot::CreateGameScene>();
    godot::register_class<godot::JoinGameScene>();
    godot::register_class<godot::Game>();
	godot::register_class<godot::Player>();
    godot::register_class<godot::Network>();
    godot::register_class<godot::Camera>();
}