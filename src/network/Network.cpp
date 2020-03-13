#include "Network.h"
#include <iostream>

#include <SceneTree.hpp>

using std::make_unique;
using namespace godot;

void Network::_register_methods()
{
    register_method("_init", &Network::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Network::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("createServer", &Network::createServer, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("joinServer", &Network::joinServer, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("closeServer", &Network::closeServer, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("_player_connected", &Network::_player_connected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_player_disconnected", &Network::_player_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_connection_successful", &Network::_connection_successful, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_connection_failed", &Network::_connection_failed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_server_disconnected", &Network::_server_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Network::_init()
{
	Godot::print("Initializing network.");
}

void Network::_ready()
{
	get_tree()->connect("network_peer_connected", this, "_player_connected");
	get_tree()->connect("network_peer_disconnected", this, "_player_disconnected");
	get_tree()->connect("connected_to_server", this, "_connection_successful");
	get_tree()->connect("connection_failed", this, "_connection_failed");
	get_tree()->connect("server_disconnected", this, "_server_disconnected");

	networkId_ = 0;
	nickname_ = "";

	Godot::print("Network ready.");
}

void Network::createServer()
{
	peer_ = make_unique<NetworkedMultiplayerENet>(*NetworkedMultiplayerENet::_new());
	peer_->create_server(SERVER_PORT, MAX_PLAYERS);
	get_tree()->set_network_peer(peer_.get());
	Godot::print("Server created successfully.");
}

void Network::joinServer(String ip)
{
	peer_ = make_unique<NetworkedMultiplayerENet>(*NetworkedMultiplayerENet::_new());
	peer_->create_client(ip, SERVER_PORT);
	get_tree()->set_network_peer(peer_.get());
	Godot::print("Server joined successfully.");
}

void Network::closeServer()
{
	get_tree()->set_network_peer(nullptr);
	peer_.reset();
	Godot::print("Server terminated.");
}

void Network::_player_connected(int64_t connectedPlayerId)
{
	Godot::print("Player" + String(connectedPlayerId) + " has connected.");
}

void Network::_player_disconnected(int64_t disconnectedPlayerId)
{
	Godot::print("Player disconnected.");
}

void Network::_connection_successful()
{
	Godot::print("Connected to server.");
}

void Network::_connection_failed()
{
	Godot::print("Could not connected to server.");
}

void Network::_server_disconnected()
{
	Godot::print("Server disconnected.");
	get_tree()->change_scene("res://scenes/MainMenu.tscn");
}