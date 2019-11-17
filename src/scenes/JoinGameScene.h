#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>
#include <NetworkedMultiplayerENet.hpp>

namespace godot
{
	class JoinGameScene : public Node2D
	{
        GODOT_CLASS(JoinGameScene, Node2D)

    private:
        std::unique_ptr<NetworkedMultiplayerENet> networkPeer_;

	public:
        JoinGameScene() = default;
		~JoinGameScene() = default;

		static void _register_methods();
		void _init();
		void _on_JoinGameButton_pressed(Variant body);
        void _on_BackButton_pressed(Variant body);
		void _on_IpAddress_text_changed(String new_text);
		void _on_PlayerName_text_changed(String new_text);

		String playerName;
		String serverIp;
	};
}