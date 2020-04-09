#pragma once
#include <memory>
#include <map>

#include <core/CoreTypes.hpp>
#include <core/Godot.hpp>
#include <Node.hpp>
#include <NetworkedMultiplayerENet.hpp>

using std::unique_ptr;
using std::map;

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

		bool createServer(String nickname);
		bool joinServer(String nickname, String ip);
		void closeNetwork();

		void _player_connected(int64_t connectedPlayerId);
		void _player_disconnected(int64_t disconnectedPlayerId);
		void _connection_successful();
		void _connection_failed();
		void _server_disconnected();

		String getPlayerNickname() const { return nickname_;  }
		String getConnectedPlayerNickname(int64_t playerId);
		int64_t getPlayerNetworkId() const { return networkId_; }
		Dictionary getConnectedPlayers() const { return connectedPlayers_; }

		void addPlayer(int64_t id, String nickname);
		void removePlayer(int64_t id);
		void sendConnectedPlayersInfo();

    private:
		void updateConnectedPlayers(Dictionary players) { connectedPlayers_ = players; }
		
		Dictionary connectedPlayers_;
		unique_ptr<NetworkedMultiplayerENet> peer_;
		int64_t networkId_;
		String nickname_;

		const int64_t SERVER_PORT = 12345;
		const int64_t MAX_PLAYERS = 128;
	};
}