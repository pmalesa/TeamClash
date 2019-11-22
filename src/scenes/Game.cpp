#include "Game.h"

#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <iostream>

#include <Ref.hpp>

using namespace godot;

void Game::_register_methods()
{
    register_method("_init", &Game::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Game::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_process", &Game::_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_on_player_disconnected", &Game::_on_player_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_on_server_disconnected", &Game::_on_server_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
}
void Game::_init()
{
    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    playerScene_ = resourceLoader->load("res://player/Player.tscn");
}

void Game::_ready()
{
    get_tree()->connect("network_peer_disconnected", this, "_on_player_disconnected");
    get_tree()->connect("server_disconnected", this, "_on_server_disconnected");

    player_ = static_cast<godot::Player*>(playerScene_->instance());

    player_->set_name(String(get_tree()->get_network_unique_id()));
    player_->set("nodeName", get_tree()->get_network_unique_id());
    player_->set_network_master(get_tree()->get_network_unique_id());
    add_child(player_);

    // Dictionary selfData = Dictionary(get_node("/root/Network")->get("selfData"));
    player_->init("Admin", Vector2(360, 180), false);
	Godot::print("Game is ready.");
}

void Game::_process(float delta)
{
    Camera* camera = static_cast<Camera*>(get_node("Camera2D"));
    camera->set_position(Vector2(player_->get_position().x, player_->get_position().y));
}

void Game::_on_player_disconnected(int64_t id)
{
    Godot::print("Player disconnected.");
    if (get_node(godot::NodePath(String(id))))
    {
        get_node(godot::NodePath(String(id)))->queue_free();
    }
}

void Game::_on_server_disconnected(int64_t id)
{
    get_tree()->change_scene("res://scenes/MainMenu.tscn");
}