#include "JoinGameScene.h"
#include <iostream>

#include <SceneTree.hpp>

using namespace godot;

void JoinGameScene::_register_methods()
{
	register_method((char*)"_init", &JoinGameScene::_init);
	register_method((char*)"_on_JoinGameButton_pressed", &JoinGameScene::_on_JoinGameButton_pressed);
    register_method((char*)"_on_BackButton_pressed", &JoinGameScene::_on_BackButton_pressed);
	register_method((char*)"_on_IpAddress_text_changed", &JoinGameScene::_on_IpAddress_text_changed);
	register_method((char*)"_on_PlayerName_text_changed", &JoinGameScene::_on_PlayerName_text_changed);
	register_method((char*)"_on_IpAddress_text_entered", &JoinGameScene::_on_IpAddress_text_entered);
	register_method((char*)"_on_PlayerName_text_entered", &JoinGameScene::_on_PlayerName_text_entered);
}

void JoinGameScene::_init()
{
	serverIp_ = "";
	playerName_ = "";
}

void JoinGameScene::_on_JoinGameButton_pressed(Variant body)
{
	std::cout << "Join game button pressed." << std::endl;
	if (playerName_ == "")
		playerName_ = "Unnamed";
	if (serverIp_.is_valid_ip_address() && static_cast<bool>(get_node("/root/Network")->call("joinServer", playerName_, serverIp_)))
		get_tree()->change_scene("res://scenes/Lobby.tscn");
}

void JoinGameScene::_on_BackButton_pressed(Variant body)
{
    std::cout << "Back button pressed." << std::endl;
    get_tree()->change_scene("res://scenes/MainMenu.tscn");
}

void JoinGameScene::_on_IpAddress_text_changed(String new_text)
{
	serverIp_ = new_text;
}

void JoinGameScene::_on_PlayerName_text_changed(String new_text)
{
	playerName_ = new_text;
}

void JoinGameScene::_on_IpAddress_text_entered(String new_text)
{
	Godot::print("New ip: " + new_text);
}

void JoinGameScene::_on_PlayerName_text_entered(String new_text)
{
	Godot::print("New name: " + new_text);
}
