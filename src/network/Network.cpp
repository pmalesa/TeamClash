#include "Network.h"
#include <iostream>

#include <SceneTree.hpp>

using std::make_unique;
using std::pair;
using namespace godot;

void Network::_register_methods()
{
    register_method("_init", &Network::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Network::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("createServer", &Network::createServer, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("joinServer", &Network::joinServer, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getPlayerNickname", &Network::getPlayerNickname, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("getConnectedPlayerNickname", &Network::getConnectedPlayerNickname, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getPlayerNetworkId", &Network::getPlayerNetworkId, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("addPlayer", &Network::addPlayer, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("removePlayer", &Network::removePlayer, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("closeNetwork", &Network::closeNetwork, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getConnectedPlayers", &Network::getConnectedPlayers, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setChosenTeam", &Network::setChosenTeam, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setChosenRole", &Network::setChosenRole, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getChosenTeam", &Network::getChosenTeam, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getChosenRole", &Network::getChosenRole, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("sendConnectedPlayersInfo", &Network::sendConnectedPlayersInfo, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("updateConnectedPlayers", &Network::updateConnectedPlayers, GODOT_METHOD_RPC_MODE_REMOTE);

	register_method("_player_connected", &Network::_player_connected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_player_disconnected", &Network::_player_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_connection_successful", &Network::_connection_successful, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_connection_failed", &Network::_connection_failed, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_server_disconnected", &Network::_server_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Network::_init()
{
	Godot::print("[NETWORK] Initializing network.");
}

void Network::_ready()
{
	get_tree()->connect("network_peer_connected", this, "_player_connected");
	get_tree()->connect("network_peer_disconnected", this, "_player_disconnected");
	get_tree()->connect("connected_to_server", this, "_connection_successful");
	get_tree()->connect("connection_failed", this, "_connection_failed");
	get_tree()->connect("server_disconnected", this, "_server_disconnected");

	nickname_ = "";
	networkId_ = 0;
	chosenTeam_ = 0;
	chosenRole_ = 0;

	Godot::print("[NETWORK] Network ready.");
}

bool Network::createServer(String nickname)
{
	peer_ = make_unique<NetworkedMultiplayerENet>(*NetworkedMultiplayerENet::_new());
	Error err = peer_->create_server(SERVER_PORT, MAX_PLAYERS);
	if (err == Error::ERR_ALREADY_IN_USE)
	{
		Godot::print("[NETWORK] Could not create server. This peer already has an open connection.");
		return false;
	}
	else if (err == Error::ERR_CANT_CREATE)
	{
		Godot::print("[NETWORK] Could not create server.");
		return false;
	}
	else if (err != Error::OK)
	{
		Godot::print("[NETWORK] Error occurred while trying to create server.");
		return false;
	}
	else
	{
		get_tree()->set_network_peer(peer_.get());
		networkId_ = get_tree()->get_network_unique_id();
		nickname_ = nickname;
		connectedPlayers_[networkId_] = nickname_;
		Godot::print("[NETWORK] Server created successfully.");
		return true;
	}
}

bool Network::joinServer(String nickname, String ip)
{
	peer_ = make_unique<NetworkedMultiplayerENet>(*NetworkedMultiplayerENet::_new());
	Error err = peer_->create_client(ip, SERVER_PORT);
	if (err == Error::ERR_CANT_CONNECT)
	{
		Godot::print("[NETWORK] Error occured. Could not connect to server.");
		return false;
	}
	else if (err == Error::ERR_CANT_RESOLVE)
	{
		Godot::print("[NETWORK] Error occured. Could not resolve hostname.");
		return false;
	}
	else if (err != Error::OK)
	{
		Godot::print("[NETWORK] Error occured while trying to connect to server.");
		return false;
	}
	else
	{
		get_tree()->set_network_peer(peer_.get());
		networkId_ = get_tree()->get_network_unique_id();
		nickname_ = nickname;
		Godot::print("[NETWORK] Server joined successfully.");
		return true;
	}
}

void Network::closeNetwork()
{
	get_tree()->set_network_peer(nullptr);
	peer_.reset();
	Godot::print("[NETWORK] Server terminated.");
}

void Network::_player_connected(int64_t connectedPlayerId)
{
	Godot::print("[NETWORK] Player connected.");
}

void Network::_player_disconnected(int64_t disconnectedPlayerId)
{
	Godot::print("[NETWORK] Player disconnected.");
}

void Network::_connection_successful()
{
	rpc_id(1, "addPlayer", networkId_, nickname_);
	Godot::print("[NETWORK] Connected to server.");
}

void Network::_connection_failed()
{
	Godot::print("[NETWORK] Could not connected to server.");
}

void Network::_server_disconnected()
{
	connectedPlayers_.clear();
	Godot::print("[NETWORK] Server disconnected.");
	get_tree()->change_scene("res://scenes/MainMenu.tscn");
}

String Network::getConnectedPlayerNickname(int64_t playerId)
{
	if (connectedPlayers_.has(playerId))
		return String(connectedPlayers_[playerId]);
	else
		return String("");
}

void Network::addPlayer(int64_t id, String nickname)
{
	connectedPlayers_[id] = nickname;
	Godot::print("[NETWORK] Added player " + nickname);
	sendConnectedPlayersInfo();
}

void Network::removePlayer(int64_t id)
{
	Godot::print("[NETWORK] Removing player " + String(connectedPlayers_[id]));
	connectedPlayers_.erase(id);
	sendConnectedPlayersInfo();
}

void Network::sendConnectedPlayersInfo()
{
	if (get_tree()->is_network_server())
	{
		rpc("updateConnectedPlayers", connectedPlayers_);
		Godot::print("[NETWORK] Updated connected players from the server.");
	}
}
