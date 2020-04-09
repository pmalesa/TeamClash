#include "CreateGameScene.h"
#include <iostream>

#include <ResourceLoader.hpp>
#include <SceneTree.hpp>

using namespace godot;

void CreateGameScene::_register_methods()
{
	register_method((char*)"_init", &CreateGameScene::_init);
	register_method((char*)"_on_PlayerName_text_changed", &CreateGameScene::_on_PlayerName_text_changed);
	register_method((char*)"_on_PlayerName_text_entered", &CreateGameScene::_on_PlayerName_text_entered);
	register_method((char*)"_on_StartGameButton_pressed", &CreateGameScene::_on_StartGameButton_pressed);
    register_method((char*)"_on_BackButton_pressed", &CreateGameScene::_on_BackButton_pressed);
}

void CreateGameScene::_init()
{
	playerName_ = "";
}

void CreateGameScene::_on_PlayerName_text_changed(String new_text)
{
	playerName_ = new_text;
}

void CreateGameScene::_on_PlayerName_text_entered(String new_text)
{
	Godot::print("New name: " + new_text);
}

void CreateGameScene::_on_StartGameButton_pressed(Variant body)
{
	std::cout << "Start game button pressed." << std::endl;
	if (playerName_ == "")
		playerName_ = "Unnamed";
	if (bool(get_node("/root/Network")->call("createServer", playerName_)))
		get_tree()->change_scene("res://scenes/Lobby.tscn");
}

void CreateGameScene::_on_BackButton_pressed(Variant body)
{
    get_tree()->change_scene("res://scenes/MainMenu.tscn");
}