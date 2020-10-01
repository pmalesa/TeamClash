#include "MenuWindow.h"

#include <iostream>
#include <SceneTree.hpp>
#include <Node2D.hpp>
#include <AudioStreamPlayer.hpp>

using namespace godot;

void MenuWindow::_register_methods()
{
	register_method("_init", &MenuWindow::_init);
	register_method("_on_ResumeButton_pressed", &MenuWindow::_on_ResumeButton_pressed);
    register_method("_on_MainMenuButton_pressed", &MenuWindow::_on_MainMenuButton_pressed);
    register_method("_on_ExitGameButton_pressed", &MenuWindow::_on_ExitGameButton_pressed);
}
void MenuWindow::_init()
{

}

void MenuWindow::_on_ResumeButton_pressed()
{
	get_node("/root/MusicModule")->call("playButtonClickSound");
	get_node("/root/Game")->call("hideMenuWindow");
}

void MenuWindow::_on_MainMenuButton_pressed()
{
	get_node("/root/MusicModule")->call("playButtonClickSound");
	get_node("/root/MusicModule")->call("stopPlayingMusic");
	get_node("/root/MusicModule")->call("playMainMenuMusic");
	get_node("/root/Game")->queue_free();
	get_node("/root/Network")->call("closeNetwork");
	get_tree()->change_scene("res://scenes/MainMenu.tscn");
}

void MenuWindow::_on_ExitGameButton_pressed()
{
	get_node("/root/MusicModule")->call("playButtonClickSound");
	get_node("/root/Game")->queue_free();
	get_node("/root/Network")->call("closeNetwork");
	get_tree()->quit();
}