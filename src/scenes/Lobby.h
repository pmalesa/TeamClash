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
	class TextureButton;

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
		void _on_EnterGameButton_pressed();
		void _on_CeladonTeamButton_pressed();
		void _on_CrimsonTeamButton_pressed();
		void _on_WarriorButton_pressed();
		void _on_ArcherButton_pressed();
		void _on_NumberOfMonstersLineEdit_text_changed(String new_text);
		void _on_NumberOfMonstersLineEdit_text_entered(String new_text);
		void _on_SpecialEventCheckBox_toggled(bool button_pressed);

		void updateConnectedPlayersWindow();
		void sendMessage(String message);
		void startGame();

	private:
		Input* keyboardInput_;
		TextEdit* connectedPlayersWindowText_;
		TextEdit* chatWindowText_;
		LineEdit* lineEdit_;
		BaseButton* sendButton_;
		TextureButton* celadonTeamButton_;
		TextureButton* crimsonTeamButton_;
		TextureButton* warriorButton_;
		TextureButton* archerButton_;

		String lobbyNickname_;
		int64_t id_;

		bool connectedPlayersOutdated_;
		bool playerJustConnected_;
		int64_t lastConnectedPlayerId_;
	};
}