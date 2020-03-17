#pragma once
#include "../player/Player.h"

#include <core/Godot.hpp>
#include <Node2D.hpp>

#include <memory>
#include <map>

using std::unique_ptr;
using std::map;

namespace godot
{
	class Input;
	class TextEdit;
	class LineEdit;
	class BaseButton;

	class Lobby : public Node2D
	{
        GODOT_CLASS(Lobby, Node2D)

	public:
		Lobby() = default;
		~Lobby() = default;

		static void _register_methods();
		void _init();
		void _ready();
		void _process(float delta);
		void _player_connected(int64_t connectedPlayerId);
		void _player_disconnected(int64_t disconnectedPlayerId);

		void _on_ChatPanelLineEdit_focus_entered();
		void _on_ChatPanelLineEdit_focus_exited();
		void _on_BackButton_pressed();
		void _on_SendButton_pressed();

		void updateConnectedPlayersWindow(Dictionary connectedPlayers);
		void sendMessage(String message);

	private:
		Input* keyboardInput_;
		TextEdit* connectedPlayersWindowText_;
		TextEdit* chatWindowText_;
		LineEdit* lineEdit_;
		BaseButton* sendButton_;

		Dictionary connectedPlayers_;
		String lobbyNickname_;
		int64_t id_;

		bool connectedPlayersOutdated_;
		bool playerConnected_;
		int64_t lastConnectedPlayerId_;
	};
}