#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>
#include <NetworkedMultiplayerENet.hpp>

namespace godot
{
	class CreateGameScene : public Node2D
	{
        GODOT_CLASS(CreateGameScene, Node2D)

	public:
		CreateGameScene() = default;
		~CreateGameScene() = default;

		static void _register_methods();
		void _init();
		void _on_PlayerName_text_changed(String new_text);
		void _on_PlayerName_text_entered(String new_text);
		void _on_StartGameButton_pressed(Variant body);
        void _on_BackButton_pressed(Variant body);

	private:
		String playerName_;
	};
}