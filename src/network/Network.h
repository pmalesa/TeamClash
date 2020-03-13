#pragma once
#include <memory>

#include <core/Godot.hpp>
#include <Node.hpp>
#include <NetworkedMultiplayerENet.hpp>

using std::unique_ptr;

namespace godot
{
	class Network : public Node
	{
        GODOT_CLASS(Network, Node)

	public:
        Network() = default;
		~Network() = default;

		static void _register_methods();
		void _init();
        void _ready();

		void createServer();
		void joinServer(String ip);
		void closeServer();

		void _player_connected(int64_t connectedPlayerId);
		void _player_disconnected(int64_t disconnectedPlayerId);
		void _connection_successful();
		void _connection_failed();
		void _server_disconnected();

		void setPlayerInfo(int64_t newNetworkId, String newNickname) { networkId_ = newNetworkId; nickname_ = newNickname; }


    private:
		unique_ptr<NetworkedMultiplayerENet> peer_;

		int64_t networkId_;
		String nickname_;

		const int64_t SERVER_PORT = 12345;
		const int64_t MAX_PLAYERS = 128;
	};
}