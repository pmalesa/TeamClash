#include "Network.h"
#include <iostream>

#include <SceneTree.hpp>
#include <NetworkedMultiplayerENet.hpp>

using namespace godot;

void Network::_register_methods()
{
    register_method("_init", &Network::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Network::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("createServer", &Network::createServer, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Network::_init()
{
    selfData_["name"] = "";
    selfData_["position"] = Vector2(360, 180);
}

void Network::_ready()
{

}

void Network::createServer(String playerName)
{
	std::cout << "Creating server." << std::endl;
    selfData_["name"] = playerName;
    players_[1] = selfData_;
    NetworkedMultiplayerENet* peer = NetworkedMultiplayerENet::_new();
    peer->create_server(12345, 128);
    get_tree()->set_network_peer(peer);
    std::cout << "Server created successfully." << std::endl;
}

void Network::update_position(int64_t id, Vector2 position)
{
    Dictionary playerInfo = players_[id];
    playerInfo["position"] = position;
}
