#include "MainMenu.h"

#include <iostream>
#include <SceneTree.hpp>

using namespace godot;

void MainMenu::_register_methods()
{
	register_method((char*)"_on_CreateGameButton_pressed", &MainMenu::_on_CreateGameButton_pressed);
}
void MainMenu::_init()
{

}

void MainMenu::_on_CreateGameButton_pressed(Variant body)
{
	std::cout << "Create button pressed." << std::endl;
	get_tree()->change_scene("res://scenes/CreateGameScene.tscn");
}