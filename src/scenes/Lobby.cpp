#include "Lobby.h"

#include <SceneTree.hpp>
#include <TextEdit.hpp>
#include <LineEdit.hpp>
#include <Input.hpp>
#include <BaseButton.hpp>

using namespace godot;
using std::make_unique;

void Lobby::_register_methods()
{
	register_method((char*)"_init", &Lobby::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_ready", &Lobby::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_process", &Lobby::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_ChatPanelLineEdit_focus_entered", &Lobby::_on_ChatPanelLineEdit_focus_entered, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_ChatPanelLineEdit_focus_exited", &Lobby::_on_ChatPanelLineEdit_focus_exited, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_BackButton_pressed", &Lobby::_on_BackButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_SendButton_pressed", &Lobby::_on_SendButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("registerPlayer", &Lobby::registerPlayer, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("sendMessage", &Lobby::sendMessage, GODOT_METHOD_RPC_MODE_REMOTESYNC);
}

void Lobby::_init()
{
	keyboardInput_ = Input::get_singleton();
}

void Lobby::_ready()
{
	connectedPlayers_.push_back(player_);
	chatWindowText_ = static_cast<godot::TextEdit*>(get_node("LobbyPanel/ChatPanel/ChatDisplay"));
	lineEdit_ = static_cast<godot::LineEdit*>(get_node("LobbyPanel/ChatPanel/ChatPanelLineEdit"));
	lineEdit_->set_placeholder_alpha(0.4);
	sendButton_ = static_cast<godot::BaseButton*>(get_node("LobbyPanel/ChatPanel/SendButton"));
	lineEdit_->set_placeholder("Enter message...");
	if (get_tree()->is_network_server())
		chatWindowText_->set_text("Lobby created.\n");
	else
		chatWindowText_->set_text("Joined lobby.\n");

	Godot::print("Lobby is ready.");
}

void Lobby::_process(float delta)
{
	if (keyboardInput_->is_action_just_pressed("enter"))
	{
		_on_SendButton_pressed();
	}
}

void Lobby::_on_ChatPanelLineEdit_focus_entered()
{
	lineEdit_->set_placeholder("");
}

void Lobby::_on_ChatPanelLineEdit_focus_exited()
{
	lineEdit_->set_placeholder("Enter message...");
}

void Lobby::_on_BackButton_pressed()
{
	get_node("/root/Network")->call("closeServer");
	get_tree()->change_scene("res://scenes/MainMenu.tscn");
}

void Lobby::_on_SendButton_pressed()
{
	if (lineEdit_->get_text() != "")
	{
		rpc("sendMessage", String(get_tree()->get_rpc_sender_id()) + lineEdit_->get_text());
	}
	Godot::print("SendButton pressed.");
}

void Lobby::registerPlayer(Player* player)
{

}

void Lobby::sendMessage(String message)
{
	chatWindowText_->cursor_set_line(chatWindowText_->cursor_get_line() + 1);
	chatWindowText_->insert_text_at_cursor(message + "\n");
	lineEdit_->set_text("");
	Godot::print("Message sent.");
}