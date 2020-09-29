#include "Game.h"

#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <Node2D.hpp>
#include <Control.hpp>
#include <Label.hpp>
#include <AudioStreamPlayer.hpp>
#include <Timer.hpp>

#include <AnimatedSprite.hpp>
#include <Sprite.hpp>
#include <Texture.hpp>
#include <Shape.hpp>
#include <CollisionShape2D.hpp>
#include <CollisionPolygon2D.hpp>
#include <RectangleShape2D.hpp>
#include <Ref.hpp>
#include <Variant.hpp>
#include <Input.hpp>
#include <CanvasLayer.hpp>

#include <OS.hpp>

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
	register_method("initiatizeCharacter", &Game::initiatizeCharacter, GODOT_METHOD_RPC_MODE_REMOTESYNC);
    register_method("_on_player_disconnected", &Game::_on_player_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_on_server_disconnected", &Game::_on_server_disconnected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("showRespawnWindow", &Game::showRespawnWindow, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("hideRespawnWindow", &Game::hideRespawnWindow, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("hideMenuWindow", &Game::hideMenuWindow, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getSelfPlayer", &Game::getSelfPlayer, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getPlayer", &Game::getPlayer, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("takeBoltFromStack", &Game::takeBoltFromStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("putBoltOnStack", &Game::putBoltOnStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("takeEarliestActivatedBolt", &Game::takeEarliestActivatedBolt, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setBoltToActivated", &Game::setBoltToActivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setBoltToDeactivated", &Game::setBoltToDeactivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getBolt", &Game::getBolt, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("takeExplosiveBoltFromStack", &Game::takeExplosiveBoltFromStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("putExplosiveBoltOnStack", &Game::putExplosiveBoltOnStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("takeEarliestActivatedExplosiveBolt", &Game::takeEarliestActivatedExplosiveBolt, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setExplosiveBoltToActivated", &Game::setExplosiveBoltToActivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setExplosiveBoltToDeactivated", &Game::setExplosiveBoltToDeactivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getExplosiveBolt", &Game::getExplosiveBolt, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("takeEntanglingBallsFromStack", &Game::takeEntanglingBallsFromStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("putEntanglingBallsOnStack", &Game::putEntanglingBallsOnStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("takeEarliestActivatedEntanglingBalls", &Game::takeEarliestActivatedEntanglingBalls, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setEntanglingBallsToActivated", &Game::setEntanglingBallsToActivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setEntanglingBallsToDeactivated", &Game::setEntanglingBallsToDeactivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getEntanglingBalls", &Game::getEntanglingBalls, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("takeTrapFromStack", &Game::takeTrapFromStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("putTrapOnStack", &Game::putTrapOnStack, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("takeEarliestActivatedTrap", &Game::takeEarliestActivatedTrap, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setTrapToActivated", &Game::setTrapToActivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setTrapToDeactivated", &Game::setTrapToDeactivated, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getTrap", &Game::getTrap, GODOT_METHOD_RPC_MODE_DISABLED);


	register_method("activateEntanglingBalls", &Game::activateEntanglingBalls, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("activateTrap", &Game::activateTrap, GODOT_METHOD_RPC_MODE_REMOTESYNC);

	register_method("removePlayerFromServer", &Game::removePlayerFromServer, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("removePlayer", &Game::removePlayer, GODOT_METHOD_RPC_MODE_REMOTESYNC);

	register_method("createScoreboardRecord", &Game::createScoreboardRecord, GODOT_METHOD_RPC_MODE_REMOTE);

	register_property<Game, int64_t>("selfPeerId_", &Game::selfPeerId_, 0, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Game::_init()
{
	ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	worldScene_ = resourceLoader->load("res://world/World.tscn");
	playerScene_ = resourceLoader->load("res://player/Player.tscn");
	boltScene_ = resourceLoader->load("res://equipment/projectiles/Bolt.tscn");
	explosiveBoltScene_ = resourceLoader->load("res://equipment/projectiles/ExplosiveBolt.tscn");
	entanglingBallsScene_ = resourceLoader->load("res://equipment/utility/EntanglingBalls.tscn");
	trapScene_ = resourceLoader->load("res://equipment/utility/Trap.tscn");
	Godot::print("[GAME] Game initialized.");
}

void Game::_ready()
{
    get_tree()->connect("network_peer_disconnected", this, "_on_player_disconnected");
    get_tree()->connect("server_disconnected", this, "_on_server_disconnected");
	respawnWindow_ = static_cast<Node2D*>(get_node("RespawnWindow"));
	menuWindow_ = static_cast<Node2D*>(get_node("MenuWindow"));
	scoreboard_ = static_cast<Scoreboard*>(get_node("ScoreboardLayer/Scoreboard"));
	hideRespawnWindow();
	hideMenuWindow();
	scoreboard_->hide();
	preconfigureGame();

	if (get_tree()->is_network_server())
		OS::get_singleton()->set_window_title(godot::String("Server - Team Clash"));
	else
		OS::get_singleton()->set_window_title(godot::String("Client - Team Clash"));

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
			Dictionary playerRecord;
			player->set_name(String(playerNetworkIds[i]));
			player->set("nodeName_", playerNetworkIds[i]);
			player->set_network_master(playerNetworkIds[i]);
			add_child(player);
			players_[playerNetworkIds[i]] = player;
		}
	}
	player_ = static_cast<Player*>(players_[selfPeerId_]);
	initializeEquipmentNodes();
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
	rpc("initiatizeCharacter", selfPeerId_, get_node("/root/Network")->call("getChosenTeam"), get_node("/root/Network")->call("getChosenRole"));
	if (get_tree()->is_network_server())
		createScoreboardRecord(selfPeerId_, get_node("/root/Network")->call("getChosenTeam"), get_node("/root/Network")->call("getChosenRole"));
	else
		rpc_id(1, "createScoreboardRecord", selfPeerId_, get_node("/root/Network")->call("getChosenTeam"), get_node("/root/Network")->call("getChosenRole"));
	get_tree()->set_pause(false);
	Godot::print("[GAME] Every player has been preconfigured.\n[GAME] The game has started.");
	static_cast<AudioStreamPlayer*>(get_node("BackgroundMusic"))->play();
}

void Game::initiatizeCharacter(int64_t peerId, int64_t chosenTeam, int64_t chosenRole)
{
	static_cast<Player*>(players_[peerId])->init(chosenTeam, chosenRole);
}

void Game::_process(float delta)
{
	Input* input = Input::get_singleton();
	Camera* camera = static_cast<Camera*>(get_node("/root/Game/Camera2D"));
	menuWindow_->set_position(player_->get_position() - Vector2(960, 540));
	camera->set_position(player_->get_position());

	if (input->is_action_just_pressed("escape"))
	{
		if (menuWindow_->is_visible())
			hideMenuWindow();
		else
			showMenuWindow();
	}

	if (!menuWindow_->is_visible())
	{
		if (input->is_action_pressed("tab"))
			scoreboard_->show();
		else
			scoreboard_->hide();
	}

	if (respawnWindow_->is_visible())
	{
		Variant timeLeft = int(static_cast<Timer*>(player_->get_node("RespawnTimer"))->get_time_left());
		static_cast<Label*>(get_node("RespawnWindow/RespawnWindowBox/CountdownLabel"))->set_text(String(timeLeft));
	}
}

void Game::_on_player_disconnected(int64_t id)
{
	Dictionary connectedPlayers = get_node("/root/Network")->call("getConnectedPlayers");
	removePlayerFromServer(id);
	get_node("/root/Network")->call("removePlayer", id);
    if (get_node(godot::NodePath(String(id))))
    {
        get_node(godot::NodePath(String(id)))->queue_free();
    }
	if (get_tree()->is_network_server())
		scoreboard_->rpc("remove", id);
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

void Game::showMenuWindow()
{
	menuWindow_->set_visible(true);
	if (player_) player_->set_process(false);
}

void Game::hideMenuWindow()
{
	menuWindow_->set_visible(false);
	if (player_) player_->set_process(true);
}

void Game::initializeEquipmentNodes()
{
	initalizeBolts();
	initializeExplosiveBolts();
	initializeEntanglingBalls();
	initializeTraps();
}

void Game::initalizeBolts()
{
	Godot::print("[GAME] Initializing bolt nodes...");
	for (unsigned int i = 0; i < MAX_BOLTS_COUNT; ++i)
	{
		Variant boltID = i;
		Bolt* bolt = static_cast<Bolt*>(boltScene_->instance());
		bolt->set_name(String(boltID));
		bolt->set_network_master(1);
		bolt->set_position(Vector2(0, 0));
		bolt->set_rotation(0);
		static_cast<CollisionPolygon2D*>(bolt->get_node("BoltArea/CollisionPolygon2D"))->set_disabled(true);
		bolt->set_process(false);
		bolt->set_physics_process(false);
		bolt->set_visible(false);
		static_cast<Node2D*>(get_node("Bolts"))->add_child(bolt);
		if (get_tree()->is_network_server())
			boltStack_.push(bolt);
		boltVector_.push_back(bolt);
	}
	Godot::print("[GAME] Bolt nodes initialized.");
}

void Game::initializeExplosiveBolts()
{
	Godot::print("[GAME] Initializing explosive bolt nodes...");
	for (unsigned int i = 0; i < MAX_EXPLOSIVE_BOLTS_COUNT; ++i)
	{
		Variant explosiveBoltID = i;
		ExplosiveBolt* explosiveBolt = static_cast<ExplosiveBolt*>(explosiveBoltScene_->instance());
		explosiveBolt->set_name(String(explosiveBoltID));
		explosiveBolt->set_network_master(1);
		explosiveBolt->set_position(Vector2(0, 0));
		explosiveBolt->set_rotation(0);
		static_cast<CollisionPolygon2D*>(explosiveBolt->get_node("ExplosiveBoltArea/CollisionPolygon2D"))->set_disabled(true);
		explosiveBolt->set_process(false);
		explosiveBolt->set_physics_process(false);
		explosiveBolt->set_visible(false);
		static_cast<Node2D*>(get_node("ExplosiveBolts"))->add_child(explosiveBolt);
		if (get_tree()->is_network_server())
			explosiveBoltStack_.push(explosiveBolt);
		explosiveBoltVector_.push_back(explosiveBolt);
	}
	Godot::print("[GAME] Explosive bolt nodes initialized.");
}

void Game::initializeEntanglingBalls()
{
	Godot::print("[GAME] Initializing entangling balls nodes...");

	for (unsigned int i = 0; i < MAX_ENTANGLING_BALLS_COUNT; ++i)
	{
		Variant entanglingBallsID = i;
		EntanglingBalls* entanglingBalls = static_cast<EntanglingBalls*>(entanglingBallsScene_->instance());
		entanglingBalls->set_name(String(entanglingBallsID));
		entanglingBalls->set_network_master(1);
		entanglingBalls->set_position(Vector2(0, 0));
		entanglingBalls->set_rotation(0);
		static_cast<CollisionPolygon2D*>(entanglingBalls->get_node("Area/CollisionPolygon2D"))->set_disabled(true);
		entanglingBalls->set_process(false);
		entanglingBalls->set_physics_process(false);
		entanglingBalls->set_visible(false);
		static_cast<Node2D*>(get_node("EntanglingBalls"))->add_child(entanglingBalls);
		if (get_tree()->is_network_server())
			entanglingBallsStack_.push(entanglingBalls);
		entanglingBallsVector_.push_back(entanglingBalls);
	}
	Godot::print("[GAME] Entangling balls nodes initialized.");
}

void Game::initializeTraps()
{
	Godot::print("[GAME] Initializing trap nodes...");
	for (unsigned int i = 0; i < MAX_TRAPS_COUNT; ++i)
	{
		Variant trapID = i;
		Trap* trap = static_cast<Trap*>(trapScene_->instance());
		trap->set_name(String(trapID));
		trap->set_network_master(1);
		static_cast<AnimatedSprite*>(trap->get_node("TrapAnimatedSprite"))->set_visible(false);
		static_cast<AnimatedSprite*>(trap->get_node("TrapAnimatedSprite"))->stop();
		static_cast<AnimatedSprite*>(trap->get_node("TrapAnimatedSprite"))->set_animation("placement");
		static_cast<AnimatedSprite*>(trap->get_node("TrapAnimatedSprite"))->set_frame(0);
		static_cast<CollisionPolygon2D*>(trap->get_node("TrapCollisionPolygon2D"))->set_disabled(true);
		static_cast<CollisionPolygon2D*>(trap->get_node("TriggerArea/TriggerCollisionPolygon2D"))->set_disabled(true);
		trap->set_process(false);
		trap->set_physics_process(false);
		trap->set_visible(false);
		trap->set_position(Vector2(0, 0));
		static_cast<Node2D*>(get_node("Traps"))->add_child(trap);
		if (get_tree()->is_network_server())
			trapStack_.push(trap);
		trapVector_.push_back(trap);
	}
	Godot::print("[GAME] Trap nodes initialized.");
}

void Game::createScoreboardRecord(int64_t playerNodeName, int64_t team, int64_t role)
{
	scoreboard_->rpc("add", playerNodeName, get_node("/root/Network")->call("getConnectedPlayerNickname", playerNodeName), 0, 0, team, role);
}

void Game::activateEntanglingBalls(String nodeName, int64_t shooterNodeName, Vector2 initialPosition, Vector2 initialDirection)
{
	Variant var = nodeName;
	int64_t id = static_cast<int64_t>(var);
	EntanglingBalls* entanglingBalls = static_cast<EntanglingBalls*>(get_node("/root/Game")->call("getEntanglingBalls", id));
	if (!entanglingBalls)
		return;
	entanglingBalls->activate(shooterNodeName, initialPosition, initialDirection);
}

void Game::activateTrap(String nodeName, String ownerNodeName, Vector2 initialPosition)
{
	Variant var = nodeName;
	int64_t id = static_cast<int64_t>(var);
	Trap* trap = static_cast<Trap*>(get_node("/root/Game")->call("getTrap", id));
	if (!trap)
		return;
	trap->activate(ownerNodeName, initialPosition);
}

Bolt* Game::takeBoltFromStack()
{
	if (boltStack_.empty())
		takeEarliestActivatedBolt()->rpc("deactivate");
	Bolt* topBolt = boltStack_.top();
	boltStack_.pop();
	return topBolt;
}

void Game::putBoltOnStack(Bolt* newBolt)
{
	boltStack_.push(newBolt);
}

Bolt* Game::takeEarliestActivatedBolt()
{
	return activatedBoltsList_.back();
}

void Game::setBoltToActivated(Bolt* bolt)
{
	activatedBoltsList_.push_front(bolt);
	activatedBoltsMap_.insert(std::pair<Bolt*, list<Bolt*>::iterator>(bolt, activatedBoltsList_.begin()));
}

void Game::setBoltToDeactivated(Bolt* bolt)
{
	activatedBoltsList_.erase(activatedBoltsMap_[bolt]);
	activatedBoltsMap_.erase(bolt);
}

ExplosiveBolt* Game::takeExplosiveBoltFromStack()
{
	if (explosiveBoltStack_.empty())
		takeEarliestActivatedExplosiveBolt()->rpc("deactivate");
	ExplosiveBolt* topExplosiveBolt = explosiveBoltStack_.top();
	explosiveBoltStack_.pop();
	return topExplosiveBolt;
}

void Game::putExplosiveBoltOnStack(ExplosiveBolt* newExplosiveBolt)
{
	explosiveBoltStack_.push(newExplosiveBolt);
}

ExplosiveBolt* Game::takeEarliestActivatedExplosiveBolt()
{
	return activatedExplosiveBoltsList_.back();
}

void Game::setExplosiveBoltToActivated(ExplosiveBolt* explosiveBolt)
{
	activatedExplosiveBoltsList_.push_front(explosiveBolt);
	activatedExplosiveBoltsMap_.insert(std::pair<ExplosiveBolt*, list<ExplosiveBolt*>::iterator>(explosiveBolt, activatedExplosiveBoltsList_.begin()));
}

void Game::setExplosiveBoltToDeactivated(ExplosiveBolt* explosiveBolt)
{
	activatedExplosiveBoltsList_.erase(activatedExplosiveBoltsMap_[explosiveBolt]);
	activatedExplosiveBoltsMap_.erase(explosiveBolt);
}

EntanglingBalls* Game::takeEntanglingBallsFromStack()
{
	if (entanglingBallsStack_.empty())
		takeEarliestActivatedEntanglingBalls()->rpc("deactivate");
	EntanglingBalls* topEntanglingBalls = entanglingBallsStack_.top();
	entanglingBallsStack_.pop();
	return topEntanglingBalls;
}
void Game::putEntanglingBallsOnStack(EntanglingBalls* newEntanglingBalls)
{
	entanglingBallsStack_.push(newEntanglingBalls);
}

EntanglingBalls* Game::takeEarliestActivatedEntanglingBalls()
{
	return activatedEntanglingBallsList_.back();
}

void Game::setEntanglingBallsToActivated(EntanglingBalls* entanglingBalls)
{
	activatedEntanglingBallsList_.push_front(entanglingBalls);
	activatedEntanglingBallsMap_.insert(std::pair<EntanglingBalls*, list<EntanglingBalls*>::iterator>(entanglingBalls, activatedEntanglingBallsList_.begin()));
}

void Game::setEntanglingBallsToDeactivated(EntanglingBalls* entanglingBalls)
{
	activatedEntanglingBallsList_.erase(activatedEntanglingBallsMap_[entanglingBalls]);
	activatedEntanglingBallsMap_.erase(entanglingBalls);
}

Trap* Game::takeTrapFromStack()
{
	if (trapStack_.empty())
		takeEarliestActivatedTrap()->rpc("deactivate");
	Trap* topTrap = trapStack_.top();
	trapStack_.pop();
	return topTrap;
}
void Game::putTrapOnStack(Trap* newTrap)
{
	trapStack_.push(newTrap);
}

Trap* Game::takeEarliestActivatedTrap()
{
	return activatedTrapsList_.back();
}

void Game::setTrapToActivated(Trap* trap)
{
	activatedTrapsList_.push_front(trap);
	activatedTrapsMap_.insert(std::pair<Trap*, list<Trap*>::iterator>(trap, activatedTrapsList_.begin()));
}

void Game::setTrapToDeactivated(Trap* trap)
{
	activatedTrapsList_.erase(activatedTrapsMap_[trap]);
	activatedTrapsMap_.erase(trap);
}

void Game::removePlayerFromServer(int64_t playerID)
{
	rpc("removePlayer", playerID);
}

void Game::removePlayer(int64_t playerID)
{
	static_cast<Player*>(players_[playerID])->queue_free();
	players_.erase(playerID);
}





















/* METHODS TO BE REMOVED */
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
/*-------------------------*/
