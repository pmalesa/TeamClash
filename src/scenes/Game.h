#pragma once
#include <core/Godot.hpp>
#include <PackedScene.hpp>

#include "../player/Player.h"
#include "../world/World.h"
#include "../camera/Camera.h"

namespace godot
{
	class Node2D;
	class Control;

	class Game : public Node
	{
		GODOT_CLASS(Game, Node)
	public:
		Game() = default;
		~Game() = default;

		static void _register_methods();
		void _init();
		void _ready();
		void preconfigureGame();
		void donePreconfiguring(int64_t peerId);
		void postconfigureGame();
		void _process(float delta);
		void _on_player_disconnected(int64_t id);
		void _on_server_disconnected(int64_t id);

		Player* getPlayer(int64_t nodeName) const { return players_[nodeName]; }

		void showRespawnWindow();
		void hideRespawnWindow();
		void showMenuWindow();
		void hideMenuWindow();

	private:
		void printAllConnectedPeers();
		void printAllConnectedPeersNodeNames();


		Ref<PackedScene> playerScene_;
		Ref<PackedScene> worldScene_;
		Player* player_;
		World* world_;

		Dictionary connectedPlayersInfo_;
		Dictionary playersDoneConfiguring_;
		Dictionary players_;
		int64_t selfPeerId_;

		Node2D* respawnWindow_;
		Node2D* menuWindow_;

		CanvasLayer* ui_;
	};
}