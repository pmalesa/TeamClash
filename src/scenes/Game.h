#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>

#include <PackedScene.hpp>

#include "../player/Player.h"
#include "../camera/Camera.h"

namespace godot
{
	class Game : public Node
	{
		GODOT_CLASS(Game, Node)
	public:
		Game() = default;
		~Game() = default;

		static void _register_methods();
		void _init();
		void _ready();
		void _process(float delta);
		void _on_player_disconnected(int64_t id);
		void _on_server_disconnected(int64_t id);
		void setPlayerNickname(String newNickname);

	private:
		Ref<PackedScene> playerScene_;
		godot::Player* player_;
	};
}