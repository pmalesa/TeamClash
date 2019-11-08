#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>

#include <PackedScene.hpp>

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
        void _on_player_disconnected(int64_t id);
        void _on_server_disconnected(int64_t id);

    private:
        Ref<PackedScene> playerScene_;

	};
}