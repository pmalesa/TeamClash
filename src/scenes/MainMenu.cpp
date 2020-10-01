#include "MainMenu.h"

#include <iostream>
#include <SceneTree.hpp>

using namespace godot;

void MainMenu::_register_methods()
{
	register_method("_init", &MainMenu::_init);
	register_method("_ready", &MainMenu::_ready);
	register_method((char*)"_on_CreateGameButton_pressed", &MainMenu::_on_CreateGameButton_pressed);
    register_method((char*)"_on_JoinGameButton_pressed", &MainMenu::_on_JoinGameButton_pressed);
    register_method((char*)"_on_ExitGameButton_pressed", &MainMenu::_on_ExitGameButton_pressed);
}
void MainMenu::_init()
{

}

void MainMenu::_ready()
{
	
}

void MainMenu::_on_CreateGameButton_pressed(Variant body)
{
	std::cout << "Create button pressed." << std::endl;
	get_node("/root/MusicModule")->call("playButtonClickSound");
	get_tree()->change_scene("res://scenes/CreateGameScene.tscn");
}

void MainMenu::_on_JoinGameButton_pressed(Variant body)
{
    std::cout << "Join button pressed." << std::endl;
	get_node("/root/MusicModule")->call("playButtonClickSound");
    get_tree()->change_scene("res://scenes/JoinGameScene.tscn");
}

void MainMenu::_on_ExitGameButton_pressed(Variant body)
{
    std::cout << "Exit button pressed." << std::endl;
	get_node("/root/MusicModule")->call("playButtonClickSound");
	get_node("/root/MusicModule")->call("stopPlayingMusic");
    get_tree()->quit();
}