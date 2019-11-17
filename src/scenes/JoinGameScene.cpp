#include "JoinGameScene.h"
#include <iostream>

#include <SceneTree.hpp>

using namespace godot;

void JoinGameScene::_register_methods()
{
	register_method((char*)"_on_JoinGameButton_pressed", &JoinGameScene::_on_JoinGameButton_pressed);
    register_method((char*)"_on_BackButton_pressed", &JoinGameScene::_on_BackButton_pressed);
	register_method((char*)"_on_IpAddress_text_changed", &JoinGameScene::_on_IpAddress_text_changed);
	register_method((char*)"_on_PlayerName_text_changed", &JoinGameScene::_on_PlayerName_text_changed);
}

void JoinGameScene::_init()
{
	serverIp = "";
}

void JoinGameScene::_on_JoinGameButton_pressed(Variant body)
{
	std::cout << "Join game button pressed." << std::endl;
    get_node("/root/Network")->call("joinServer", playerName, serverIp);
    get_tree()->change_scene("res://scenes/Game.tscn");
}

void JoinGameScene::_on_BackButton_pressed(Variant body)
{
    std::cout << "Back button pressed." << std::endl;
    get_tree()->change_scene("res://scenes/MainMenu.tscn");
}

void JoinGameScene::_on_IpAddress_text_changed(String new_text)
{
	serverIp = new_text;
}

void JoinGameScene::_on_PlayerName_text_changed(String new_text)
{
	playerName = new_text;
}