#include "CreateGameScene.h"
#include <iostream>

using namespace godot;

void CreateGameScene::_register_methods()
{
	register_method((char*)"_on_StartGameButton_pressed", &CreateGameScene::_on_StartGameButton_pressed);
}

void CreateGameScene::_init()
{

}

void CreateGameScene::_on_StartGameButton_pressed(Variant body)
{
	std::cout << "Start game button pressed." << std::endl;
}
