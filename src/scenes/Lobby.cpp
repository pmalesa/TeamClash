#include "Lobby.h"

#include <SceneTree.hpp>
#include <TextEdit.hpp>
#include <LineEdit.hpp>
#include <Input.hpp>
#include <BaseButton.hpp>
#include <TextureButton.hpp>

using namespace godot;

void Lobby::_register_methods()
{
	register_method((char*)"_init", &Lobby::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_ready", &Lobby::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_process", &Lobby::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_player_connected", &Lobby::_player_connected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_player_disconnected", &Lobby::_player_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_ChatPanelLineEdit_focus_entered", &Lobby::_on_ChatPanelLineEdit_focus_entered, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_ChatPanelLineEdit_focus_exited", &Lobby::_on_ChatPanelLineEdit_focus_exited, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_BackButton_pressed", &Lobby::_on_BackButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_SendButton_pressed", &Lobby::_on_SendButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method((char*)"_on_EnterGameButton_pressed", &Lobby::_on_EnterGameButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_CeladonTeamButton_pressed", &Lobby::_on_CeladonTeamButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_CrimsonTeamButton_pressed", &Lobby::_on_CrimsonTeamButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_WarriorButton_pressed", &Lobby::_on_WarriorButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ArcherButton_pressed", &Lobby::_on_ArcherButton_pressed, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("updateConnectedPlayersWindow", &Lobby::updateConnectedPlayersWindow, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("sendMessage", &Lobby::sendMessage, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("startGame", &Lobby::startGame, GODOT_METHOD_RPC_MODE_REMOTESYNC);
}

void Lobby::_init()
{
	keyboardInput_ = Input::get_singleton();
	connectedPlayersOutdated_ = false;
	playerJustConnected_ = false;
	lastConnectedPlayerId_ = 0;
}

void Lobby::_ready()
{
	get_tree()->connect("network_peer_connected", this, "_player_connected");
	get_tree()->connect("network_peer_disconnected", this, "_player_disconnected");

	lobbyNickname_ = get_node("/root/Network")->call("getPlayerNickname");
	id_ = get_node("/root/Network")->call("getPlayerNetworkId");

	connectedPlayersWindowText_ = static_cast<godot::TextEdit*>(get_node("LobbyPanel/ConnectedPlayersPanel/ConnectedPlayersDisplay"));
	chatWindowText_ = static_cast<godot::TextEdit*>(get_node("LobbyPanel/ChatPanel/ChatDisplay"));
	lineEdit_ = static_cast<godot::LineEdit*>(get_node("LobbyPanel/ChatPanel/ChatPanelLineEdit"));
	lineEdit_->set_placeholder_alpha(0.4);
	sendButton_ = static_cast<godot::BaseButton*>(get_node("LobbyPanel/ChatPanel/SendButton"));
	lineEdit_->set_placeholder("Enter message...");
	celadonTeamButton_ = static_cast<TextureButton*>(get_node("LobbyPanel/ConfigWindow/CeladonTeamButton"));
	crimsonTeamButton_ = static_cast<TextureButton*>(get_node("LobbyPanel/ConfigWindow/CrimsonTeamButton"));
	warriorButton_ = static_cast<TextureButton*>(get_node("LobbyPanel/ConfigWindow/WarriorButton"));
	archerButton_ = static_cast<TextureButton*>(get_node("LobbyPanel/ConfigWindow/ArcherButton"));

	if (get_tree()->is_network_server())
	{
		chatWindowText_->set_text("[INFO] Lobby created.\n");
		updateConnectedPlayersWindow();
	}
	else
	{
		static_cast<BaseButton*>(get_node("LobbyPanel/EnterGameButton"))->set_disabled(true);
		static_cast<BaseButton*>(get_node("LobbyPanel/EnterGameButton"))->hide();
	}
	Godot::print("[LOBBY] Lobby is ready.");
}

void Lobby::_process(float delta)
{
	if (get_tree()->is_network_server() && connectedPlayersOutdated_)
	{
		rpc("updateConnectedPlayersWindow");
		connectedPlayersOutdated_ = false;
	}
	if (get_tree()->is_network_server() && playerJustConnected_)
	{
		Dictionary connectedPlayers = get_node("/root/Network")->call("getConnectedPlayers");
		rpc("sendMessage", String(connectedPlayers[lastConnectedPlayerId_]) + " has joined the lobby.");
		playerJustConnected_ = false;
	}
	if (keyboardInput_->is_action_just_pressed("enter"))
	{
		_on_SendButton_pressed();
	}
}

void Lobby::_player_connected(int64_t connectedPlayerId)
{
	if (get_tree()->is_network_server())
	{
		lastConnectedPlayerId_ = connectedPlayerId;
		playerJustConnected_ = true;
		connectedPlayersOutdated_ = true;
	}
	Godot::print("[LOBBY] Player connected.");
}

void Lobby::_player_disconnected(int64_t disconnectedPlayerId)
{
	if (get_tree()->is_network_server())
	{
		Dictionary connectedPlayers = get_node("/root/Network")->call("getConnectedPlayers");
		rpc("sendMessage", String(connectedPlayers[disconnectedPlayerId]) + " has left the lobby.");
		get_node("/root/Network")->call("removePlayer", disconnectedPlayerId);
		connectedPlayersOutdated_ = true;
	}
	Godot::print("[LOBBY] Player disconnected.");
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
	get_node("/root/Network")->call("closeNetwork");
	get_tree()->change_scene("res://scenes/MainMenu.tscn");
	queue_free();
}

void Lobby::_on_SendButton_pressed()
{
	if (lineEdit_->get_text() != "")
	{
		rpc("sendMessage", String("[ ") + lobbyNickname_ + String(" ]: ") + lineEdit_->get_text());
		lineEdit_->set_text("");
	}
	Godot::print("[LOBBY] SendButton pressed.");
}

void Lobby::_on_EnterGameButton_pressed()
{
	if (get_tree()->is_network_server())
	{
		get_tree()->set_refuse_new_network_connections(true);
		rpc("startGame");
		queue_free();
	}
}

void Lobby::_on_CeladonTeamButton_pressed()
{
	get_node("/root/Network")->call("setChosenTeam", 0);
	celadonTeamButton_->set_pressed(true);
	crimsonTeamButton_->set_pressed(false);
}

void Lobby::_on_CrimsonTeamButton_pressed()
{
	get_node("/root/Network")->call("setChosenTeam", 1);
	celadonTeamButton_->set_pressed(false);
	crimsonTeamButton_->set_pressed(true);
}

void Lobby::_on_WarriorButton_pressed()
{
	get_node("/root/Network")->call("setChosenRole", 0);
	warriorButton_->set_pressed(true);
	archerButton_->set_pressed(false);
}

void Lobby::_on_ArcherButton_pressed()
{
	get_node("/root/Network")->call("setChosenRole", 1);
	warriorButton_->set_pressed(false);
	archerButton_->set_pressed(true);
}

void Lobby::updateConnectedPlayersWindow()
{
	Dictionary connectedPlayers = get_node("/root/Network")->call("getConnectedPlayers");
	connectedPlayersWindowText_->set_text("");
	Array connectedPlayersIds = connectedPlayers.keys();
	for (int i = 0; i < connectedPlayersIds.size(); ++i)
	{
		connectedPlayersWindowText_->cursor_set_line(connectedPlayersWindowText_->cursor_get_line() + 1);
		connectedPlayersWindowText_->insert_text_at_cursor(String(connectedPlayers[connectedPlayersIds[i]]) + "	(" + String(connectedPlayersIds[i]) + ")" + "\n");
	}
}

void Lobby::sendMessage(String message)
{
	chatWindowText_->cursor_set_line(chatWindowText_->cursor_get_line() + 1);
	chatWindowText_->insert_text_at_cursor(message + "\n");
	Godot::print("[LOBBY] Message sent.");
}

void Lobby::startGame()
{
	get_tree()->change_scene("res://scenes/Game.tscn");
}



