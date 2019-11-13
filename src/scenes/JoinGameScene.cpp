#include "JoinGameScene.h"
#include <iostream>

#include <SceneTree.hpp>

using namespace godot;

void JoinGameScene::_register_methods()
{
	register_method((char*)"_on_JoinGameButton_pressed", &JoinGameScene::_on_JoinGameButton_pressed);
    register_method((char*)"_on_BackButton_pressed", &JoinGameScene::_on_BackButton_pressed);
}

void JoinGameScene::_init()
{

}

void JoinGameScene::_on_JoinGameButton_pressed(Variant body)
{
	std::cout << "Join game button pressed." << std::endl;
    String playerName = "Client";
    get_node("/root/Network")->call("joinServer", playerName);
    get_tree()->change_scene("res://scenes/Game.tscn");
}

void JoinGameScene::_on_BackButton_pressed(Variant body)
{
    std::cout << "Back button pressed." << std::endl;
    get_tree()->change_scene("res://scenes/MainMenu.tscn");
}
