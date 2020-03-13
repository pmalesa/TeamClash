#pragma once
#include "../player/Player.h"

#include <core/Godot.hpp>
#include <Node2D.hpp>

#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

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

		void _on_ChatPanelLineEdit_focus_entered();
		void _on_ChatPanelLineEdit_focus_exited();
		void _on_BackButton_pressed();
		void _on_SendButton_pressed();

		void registerPlayer(Player* player);
		void setNickname(String newNickname) { player_->setNickname(newNickname); }

		void sendMessage(String message);

	private:
		vector<Player*> connectedPlayers_;

		Input* keyboardInput_;
		TextEdit* chatWindowText_;
		LineEdit* lineEdit_;
		BaseButton* sendButton_;
		godot::Player* player_;
	};
}