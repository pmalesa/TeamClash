//#include "Network.h"
//#include <iostream>
//
//#include <SceneTree.hpp>
//#include <NetworkedMultiplayerENet.hpp>
//
//#include <ResourceLoader.hpp>
//#include "../player/Player.h"
//
//using namespace godot;
//
//void Network::_register_methods()
//{
//    register_method("_init", &Network::_init, GODOT_METHOD_RPC_MODE_DISABLED);
//    register_method("_ready", &Network::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
//    register_method("createServer", &Network::createServer, GODOT_METHOD_RPC_MODE_DISABLED);
//    register_method("joinServer", &Network::joinServer, GODOT_METHOD_RPC_MODE_DISABLED);
//    register_method("_on_player_disconnected", &Network::_on_player_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
//    register_method("_on_player_connected", &Network::_on_player_connected, GODOT_METHOD_RPC_MODE_DISABLED);
//	register_method("_connected_to_server", &Network::_connected_to_server, GODOT_METHOD_RPC_MODE_DISABLED);
//	register_method("_request_player_info", &Network::_request_player_info, GODOT_METHOD_RPC_MODE_REMOTE);
//	//register_method("_request_players", &Network::_request_players, GODOT_METHOD_RPC_MODE_REMOTE);
//    register_method("_send_player_info", &Network::_send_player_info, GODOT_METHOD_RPC_MODE_REMOTE);
//    register_method("update_position", &Network::update_position, GODOT_METHOD_RPC_MODE_DISABLED);
//
//	register_property<Network, Dictionary>("selfData", &Network::selfData_, Dictionary(), GODOT_METHOD_RPC_MODE_DISABLED);
//	register_property<Network, Dictionary>("players", &Network::players_, Dictionary(), GODOT_METHOD_RPC_MODE_DISABLED);
//}
//
//void Network::_init()
//{
//    selfData_["name"] = "";
//    selfData_["position"] = Vector2(360, 180);
//}
//
//void Network::_ready()
//{
//    get_tree()->connect("network_peer_disconnected", this, "_on_player_disconnected");
//    get_tree()->connect("network_peer_connected", this, "_on_player_connected");
//}
//
//void Network::createServer(String playerName)
//{
//	std::cout << "Creating server." << std::endl;
//    selfData_["name"] = playerName;
//    players_[1] = selfData_;
//    NetworkedMultiplayerENet* peer = NetworkedMultiplayerENet::_new();
//    peer->create_server(SERVER_PORT, 128);
//    get_tree()->set_network_peer(peer);
//    std::cout << "Server created successfully." << std::endl;
//}
//
//void Network::joinServer(String playerName, String ip)
//{
//    std::cout << "Joining server." << std::endl;
//    selfData_["name"] = "Client";
//    get_tree()->connect("connected_to_server", this, "_connected_to_server");
//    NetworkedMultiplayerENet* peer = NetworkedMultiplayerENet::_new();
//    peer->create_client(ip, SERVER_PORT);
//    get_tree()->set_network_peer(peer);
//    std::cout << "Server joined successfully." << std::endl;
//}
//
//void Network::_connected_to_server()
//{
//    int64_t localPlayerId = get_tree()->get_network_unique_id();
//    players_[localPlayerId] = selfData_;
//    rpc("_send_player_info", localPlayerId, selfData_);
//}
//
//void Network::_on_player_disconnected(int64_t id)
//{
//    players_.erase(id);
//}
//
//void Network::_on_player_connected(int64_t connectedPlayerId)
//{
//    std::cout << "Player connected to server" << std::endl;
//    int64_t localPlayerId = get_tree()->get_network_unique_id();
//    if (!get_tree()->is_network_server())
//    {
//        rpc_id(1, "_request_player_info", localPlayerId, connectedPlayerId);
//    }
//}
//
//void Network::_request_player_info(int64_t requestFromId, int64_t playerId)
//{
//	if (get_tree()->is_network_server())
//	{
//		rpc_id(requestFromId, "_send_player_info", playerId, players_[playerId]);
//	}
//}
//
//void Network::_send_player_info(int64_t id, Dictionary info)
//{
//    players_[id] = info;
//
//    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
//    playerScene_ = resourceLoader->load("res://player/Player.tscn");
//    godot::Player* player = static_cast<godot::Player*>(playerScene_->instance());
//    player->set_name(String(id));
//    player->set_network_master(id);
//    get_node("/root/Game")->add_child(player);
//    player->init(info["name"], info["position"], true);
//}
//
//void Network::update_position(int64_t id, Vector2 position)
//{
//    Dictionary playerInfo = players_[id];
//    playerInfo["position"] = position;
//}
