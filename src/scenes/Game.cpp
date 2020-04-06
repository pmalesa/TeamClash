#include "Game.h"

#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <Label.hpp>

#include <Sprite.hpp>
#include <Texture.hpp>
#include <Shape.hpp>
#include <CollisionShape2D.hpp>
#include <RectangleShape2D.hpp>

#include <Ref.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void Game::_register_methods()
{
    register_method("_init", &Game::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Game::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Game::_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("preconfigureGame", &Game::preconfigureGame, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("donePreconfiguring", &Game::donePreconfiguring, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("postconfigureGame", &Game::postconfigureGame, GODOT_METHOD_RPC_MODE_REMOTESYNC);
    register_method("_on_player_disconnected", &Game::_on_player_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_on_server_disconnected", &Game::_on_server_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getPlayer", &Game::getPlayer, GODOT_METHOD_RPC_MODE_DISABLED);
}
void Game::_init()
{
	ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	worldScene_ = resourceLoader->load("res://world/World.tscn");
	playerScene_ = resourceLoader->load("res://player/Player.tscn");
	Godot::print("Game initialized.");
}

void Game::_ready()
{
    get_tree()->connect("network_peer_disconnected", this, "_on_player_disconnected");
    get_tree()->connect("server_disconnected", this, "_on_server_disconnected");
	preconfigureGame();
	Godot::print("Game is ready.");
}

void Game::preconfigureGame()
{
	get_tree()->set_pause(true);
	connectedPlayersInfo_ = get_node("/root/Network")->call("getConnectedPlayers");
	selfPeerId_ = get_tree()->get_network_unique_id();
	world_ = static_cast<godot::World*>(worldScene_->instance());
	add_child(world_);

	/* Load players */
	Array playerNetworkIds = connectedPlayersInfo_.keys();
	for (int i = 0; i < playerNetworkIds.size(); ++i)
	{
		if (!players_.has(playerNetworkIds[i]))
		{
			Player* player = static_cast<godot::Player*>(playerScene_->instance());
			player->set_name(String(playerNetworkIds[i]));
			player->set("nodeName", playerNetworkIds[i]);
			player->set_network_master(playerNetworkIds[i]);
			add_child(player);
			player->init(connectedPlayersInfo_[playerNetworkIds[i]], Vector2(360, 180), false);
			players_[playerNetworkIds[i]] = player;
		}
	}
	player_ = players_[selfPeerId_];
	if (!get_tree()->is_network_server())
		rpc_id(1, "donePreconfiguring", selfPeerId_);
	else
		donePreconfiguring(selfPeerId_);
}

void Game::donePreconfiguring(int64_t peerId)
{
	if (get_tree()->is_network_server())
	{
		if (!playersDoneConfiguring_.has(peerId))
		{
			playersDoneConfiguring_[peerId] = connectedPlayersInfo_[peerId];
			Godot::print("Player " + String(playersDoneConfiguring_[peerId]) + " has finished preconfiguring.");
		}
		if (!playersDoneConfiguring_.has_all(connectedPlayersInfo_.keys()))
			Godot::print("Waiting for others to preconfigure...");
		else
			rpc("postconfigureGame");
	}
}

void Game::postconfigureGame()
{
	get_tree()->set_pause(false);
	Godot::print("Every player has been preconfigured.\nThe game has started.");
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

void Game::printAllConnectedPeers()
{
	Array ids = connectedPlayersInfo_.keys();

	Godot::print("**** List of all connected players' IDs:");
	for (unsigned int i = 0; i < ids.size(); ++i)
	{
		Godot::print("**** " + String(ids[i]));
	}
}

void Game::printAllConnectedPeersNodeNames()
{
	Array ids = players_.keys();

	Godot::print("#### List of all connected players' node names:");
	for (unsigned int i = 0; i < ids.size(); ++i)
	{
		Player* p = players_[ids[i]];
		Godot::print("#### " + p->get_name());
	}
}