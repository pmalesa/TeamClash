#pragma once
#include <core/Godot.hpp>
#include <Node.hpp>
#include <PackedScene.hpp>

namespace godot
{
	class Network : public Node
	{
        GODOT_CLASS(Network, Node)

	public:
        Network() = default;
		~Network() = default;

		static void _register_methods();
		void _init();
        void _ready();

        void createServer(String playerName);
        void joinServer(String playerName);

        void _connected_to_server();
        void _on_player_disconnected(int64_t id);
        void _on_player_connected(int64_t connectedPlayerId);
        void _send_player_info(int64_t id, Dictionary info);
        void update_position(int64_t id, Vector2 position);

    private:
        Ref<PackedScene> playerScene_;

        Dictionary selfData_;
        Dictionary players_;


	};
}