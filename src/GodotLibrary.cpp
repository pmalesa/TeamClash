#include "scenes/MainMenu.h"
#include "scenes/CreateGameScene.h"
#include "scenes/JoinGameScene.h"
#include "scenes/Game.h"
#include "scenes/Lobby.h"
#include "scenes/MenuWindow.h"
#include "scenes/Scoreboard.h"
#include "player/Player.h"
#include "player/ui/HealthBar.h"
#include "player/ui/UI.h"
#include "player/ui/WarriorUI.h"
#include "player/ui/ArcherUI.h"
#include "camera/Camera.h"
#include "equipment/weapons/Weapon.h"
#include "equipment/weapons/Dagger.h"
#include "equipment/weapons/Axe.h"
#include "equipment/weapons/Crossbow.h"
#include "equipment/projectiles/Bolt.h"
#include "equipment/projectiles/ExplosiveBolt.h"
#include "equipment/utility/EntanglingBalls.h"
#include "equipment/utility/Trap.h"

#include "world/Block.h"
#include "world/Biome.h"
#include "world/World.h"

#include "network/Network.h"
#include "music_module/MusicModule.h"

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
	godot::register_class<godot::Lobby>();
	godot::register_class<godot::MenuWindow>();
	godot::register_class<godot::Scoreboard>();
	godot::register_class<godot::Game>();
	godot::register_class<godot::Player>();
	godot::register_class<godot::HealthBar>();
	godot::register_class<godot::UI>();
	godot::register_class<godot::WarriorUI>();
	godot::register_class<godot::ArcherUI>();
	godot::register_class<godot::Network>();
	godot::register_class<godot::MusicModule>();
	godot::register_class<godot::Camera>();
	godot::register_class<godot::Weapon>();
	godot::register_class<godot::Dagger>();
	godot::register_class<godot::Axe>();
	godot::register_class<godot::Crossbow>();
	godot::register_class<godot::Bolt>();
	godot::register_class<godot::ExplosiveBolt>();
	godot::register_class<godot::EntanglingBalls>();
	godot::register_class<godot::Trap>();
	godot::register_class<godot::Block>();
	godot::register_class<godot::World>();
	godot::register_class<godot::Biome>();
}