#pragma once
#include <core/Godot.hpp>
#include <PackedScene.hpp>

#include "../player/Player.h"
#include "../world/World.h"
#include "../camera/Camera.h"
#include "Scoreboard.h"

#include "../equipment/projectiles/Bolt.h"
#include "../equipment/projectiles/ExplosiveBolt.h"
#include "../equipment/utility/EntanglingBalls.h"
#include "../equipment/utility/Trap.h"

#include <vector>
#include <stack>
#include <list>
#include <unordered_map>

using std::vector;
using std::stack;
using std::list;
using std::unordered_map;

namespace godot
{
	class Node2D;

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
		void initiatizeCharacter(int64_t selfPeerId, int64_t chosenTeam, int64_t chosenRole);
		void _process(float delta);
		void _on_player_disconnected(int64_t id);
		void _on_server_disconnected(int64_t id);

		Player* getSelfPlayer() const { return player_; }
		Player* getPlayer(int64_t nodeName) const { return players_[nodeName]; }
		int64_t getSelfPeerID() const { return selfPeerId_; }

		void showRespawnWindow();
		void hideRespawnWindow();
		void showMenuWindow();
		void hideMenuWindow();

		Bolt* getBolt(int64_t id) { return boltVector_[id]; }
		ExplosiveBolt* getExplosiveBolt(int64_t id) { return explosiveBoltVector_[id]; }
		EntanglingBalls* getEntanglingBalls(int64_t id) { return entanglingBallsVector_[id]; }
		Trap* getTrap(int64_t id) { return trapVector_[id]; }

	private:
		void initializeEquipmentNodes();
		void initalizeBolts();
		void initializeExplosiveBolts();
		void initializeEntanglingBalls();
		void initializeTraps();

		void initalizeMonsters();
		void initializeImps(uint16_t nImps);

		void createScoreboardRecord(int64_t playerNodeName, int64_t team, int64_t role);

		void activateEntanglingBalls(String nodeName, int64_t shooterNodeName, Vector2 initialPosition, Vector2 initialDirection);
		void activateTrap(String nodeName, String ownerNodeName, Vector2 initialPosition);

		/* Server methods */
		void removePlayerFromServer(int64_t playerID);
		void removePlayer(int64_t playerID);

		Bolt* takeBoltFromStack();
		void putBoltOnStack(Bolt* newBolt);
		Bolt* takeEarliestActivatedBolt();
		void setBoltToActivated(Bolt* bolt);
		void setBoltToDeactivated(Bolt* bolt);

		ExplosiveBolt* takeExplosiveBoltFromStack();
		void putExplosiveBoltOnStack(ExplosiveBolt* newExplosiveBolt);
		ExplosiveBolt* takeEarliestActivatedExplosiveBolt();
		void setExplosiveBoltToActivated(ExplosiveBolt* explosiveBolt);
		void setExplosiveBoltToDeactivated(ExplosiveBolt* explosiveBolt);

		EntanglingBalls* takeEntanglingBallsFromStack();
		void putEntanglingBallsOnStack(EntanglingBalls* newEntanglingBalls);
		EntanglingBalls* takeEarliestActivatedEntanglingBalls();
		void setEntanglingBallsToActivated(EntanglingBalls* entanglingBalls);
		void setEntanglingBallsToDeactivated(EntanglingBalls* entanglingBalls);

		Trap* takeTrapFromStack();
		void putTrapOnStack(Trap* newTrap);
		Trap* takeEarliestActivatedTrap();
		void setTrapToActivated(Trap* trap);
		void setTrapToDeactivated(Trap* trap);
		/*------------------------------*/

		/* Methods that will be removed */
		void printAllConnectedPeers();
		void printAllConnectedPeersNodeNames();
		/*------------------------------*/
		

		Ref<PackedScene> playerScene_;
		Ref<PackedScene> worldScene_;
		Player* player_;
		World* world_;

		Dictionary connectedPlayersInfo_;
		Dictionary playersDoneConfiguring_;
		Dictionary players_;
		int64_t selfPeerId_;

		Ref<PackedScene> boltScene_;
		Ref<PackedScene> explosiveBoltScene_;
		Ref<PackedScene> entanglingBallsScene_;
		Ref<PackedScene> trapScene_;

		Ref<PackedScene> impScene_;
		uint16_t nImps_;

		/* PRECONSTRUCTED OBJECTS */
		const static uint64_t MAX_BOLTS_COUNT = 100;
		const static uint64_t MAX_EXPLOSIVE_BOLTS_COUNT = 100;
		const static uint64_t MAX_ENTANGLING_BALLS_COUNT = 64;
		const static uint64_t MAX_TRAPS_COUNT = 64;

		stack<Bolt*> boltStack_;
		vector<Bolt*> boltVector_;
		unordered_map<Bolt*, list<Bolt*>::iterator> activatedBoltsMap_;
		list<Bolt*> activatedBoltsList_;

		stack<ExplosiveBolt*> explosiveBoltStack_;
		vector<ExplosiveBolt*> explosiveBoltVector_;
		unordered_map<ExplosiveBolt*, list<ExplosiveBolt*>::iterator> activatedExplosiveBoltsMap_;
		list<ExplosiveBolt*> activatedExplosiveBoltsList_;

		stack<EntanglingBalls*> entanglingBallsStack_;
		vector<EntanglingBalls*> entanglingBallsVector_;
		unordered_map<EntanglingBalls*, list<EntanglingBalls*>::iterator> activatedEntanglingBallsMap_;
		list<EntanglingBalls*> activatedEntanglingBallsList_;

		stack<Trap*> trapStack_;
		vector<Trap*> trapVector_;
		unordered_map<Trap*, list<Trap*>::iterator> activatedTrapsMap_;
		list<Trap*> activatedTrapsList_;

		Node2D* respawnWindow_;
		Node2D* menuWindow_;
		Scoreboard* scoreboard_;
	};
}