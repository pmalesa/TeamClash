#include "Game.h"

#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <Node2D.hpp>
#include <Control.hpp>
#include <Label.hpp>
#include <AudioStreamPlayer.hpp>
#include <Timer.hpp>

#include <Sprite.hpp>
#include <Texture.hpp>
#include <Shape.hpp>
#include <CollisionShape2D.hpp>
#include <RectangleShape2D.hpp>
#include <Ref.hpp>
#include <Variant.hpp>

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
	register_method("showRespawnWindow", &Game::showRespawnWindow, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("hideRespawnWindow", &Game::hideRespawnWindow, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<Game, int64_t>("selfPeerId_", &Game::selfPeerId_, 0, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Game::_init()
{
	ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	worldScene_ = resourceLoader->load("res://world/World.tscn");
	playerScene_ = resourceLoader->load("res://player/Player.tscn");
	Godot::print("[GAME] Game initialized.");
}

void Game::_ready()
{
    get_tree()->connect("network_peer_disconnected", this, "_on_player_disconnected");
    get_tree()->connect("server_disconnected", this, "_on_server_disconnected");
	respawnWindow_ = static_cast<Node2D*>(get_node("RespawnWindow"));
	hideRespawnWindow();
	preconfigureGame();
	Godot::print("[GAME] Game is ready.");
}

void Game::preconfigureGame()
{
	get_tree()->set_pause(true);
	connectedPlayersInfo_ = get_node("/root/Network")->call("getConnectedPlayers");
	selfPeerId_ = get_tree()->get_network_unique_id();
	world_ = static_cast<World*>(worldScene_->instance());
	add_child(world_);

	/* Load players */
	Array playerNetworkIds = connectedPlayersInfo_.keys();
	for (int i = 0; i < playerNetworkIds.size(); ++i)
	{
		if (!players_.has(playerNetworkIds[i]))
		{
			Godot::print("[GAME] Initializing player " + String(connectedPlayersInfo_[playerNetworkIds[i]]) + " in progress...");
			Player* player = static_cast<Player*>(playerScene_->instance());
			player->set_name(String(playerNetworkIds[i]));
			player->set("nodeName", playerNetworkIds[i]);
			player->set_network_master(playerNetworkIds[i]);
			add_child(player);
			player->init(connectedPlayersInfo_[playerNetworkIds[i]], Vector2(360, 180), false);
			players_[playerNetworkIds[i]] = player;
		}
	}
	player_ = static_cast<Player*>(players_[selfPeerId_]);
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
			Godot::print("[GAME] Player " + String(playersDoneConfiguring_[peerId]) + " has finished preconfiguring.");
		}
		if (!playersDoneConfiguring_.has_all(connectedPlayersInfo_.keys()))
			Godot::print("[GAME] Waiting for others to preconfigure...");
		else
			rpc("postconfigureGame");
	}
}

void Game::postconfigureGame()
{
	get_tree()->set_pause(false);
	Godot::print("[GAME] Every player has been preconfigured.\nThe game has started.");
	static_cast<AudioStreamPlayer*>(get_node("BackgroundMusic"))->play();
}

void Game::_process(float delta)
{
    Camera* camera = static_cast<Camera*>(get_node("Camera2D"));
    camera->set_position(Vector2(player_->get_position().x, player_->get_position().y));

	if (respawnWindow_->is_visible())
	{
		Variant timeLeft = int(static_cast<Timer*>(player_->get_node("RespawnTimer"))->get_time_left());
		static_cast<Label*>(get_node("RespawnWindow/RespawnWindowBox/CountdownLabel"))->set_text(String(timeLeft));
	}
}

void Game::_on_player_disconnected(int64_t id)
{
	Dictionary connectedPlayers = get_node("/root/Network")->call("getConnectedPlayers");
	get_node("/root/Network")->call("removePlayer", id);
    if (get_node(godot::NodePath(String(id))))
    {
        get_node(godot::NodePath(String(id)))->queue_free();
    }
	Godot::print("Player disconnected.");
}

void Game::_on_server_disconnected(int64_t id)
{
    get_tree()->change_scene("res://scenes/MainMenu.tscn");
}

void Game::showRespawnWindow()
{
	Camera* camera = static_cast<Camera*>(get_node("Camera2D"));
	respawnWindow_->set_position(Vector2(camera->get_position().x - 960, camera->get_position().y - 540));
	respawnWindow_->set_visible(true);
}

void Game::hideRespawnWindow()
{
	respawnWindow_->set_visible(false);
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