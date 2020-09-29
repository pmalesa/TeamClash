#include "Scoreboard.h"

#include "../player/Player.h"

#include <ResourceLoader.hpp>
#include <VBoxContainer.hpp>
#include <Label.hpp>

using std::pair;

using namespace godot;

void Scoreboard::_register_methods()
{
	register_method("_init", &Scoreboard::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &Scoreboard::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("add", &Scoreboard::add, GODOT_METHOD_RPC_MODE_REMOTESYNC);

	register_method("remove", &Scoreboard::remove, GODOT_METHOD_RPC_MODE_REMOTESYNC);

	register_method("incrementKillCount", &Scoreboard::incrementKillCount, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("incrementDeathCount", &Scoreboard::incrementDeathCount, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("decrementKillCount", &Scoreboard::decrementKillCount, GODOT_METHOD_RPC_MODE_REMOTE);

	register_method("createScoreboardRecord", &Scoreboard::createScoreboardRecord, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updatePlayerScore", &Scoreboard::updatePlayerScore, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateScoreboardRecord", &Scoreboard::updateScoreboardRecord, GODOT_METHOD_RPC_MODE_REMOTESYNC);

	register_property<Scoreboard, int64_t>("celadonTeamScore_", &Scoreboard::celadonTeamScore_, 0, GODOT_METHOD_RPC_MODE_REMOTE);
	register_property<Scoreboard, int64_t>("crimsonTeamScore_", &Scoreboard::crimsonTeamScore_, 0, GODOT_METHOD_RPC_MODE_REMOTE);
}
void Scoreboard::_init()
{

}

void Scoreboard::_ready()
{
	ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	celadonRecordScene_ = resourceLoader->load("res://scenes/ScoreboardScenes/CeladonPlayerRecord.tscn");
	crimsonRecordScene_ = resourceLoader->load("res://scenes/ScoreboardScenes/CrimsonPlayerRecord.tscn");
	celadonTeamScore_ = 0;
	crimsonTeamScore_ = 0;
	Variant initialScore = 0;
	static_cast<Label*>(get_node("CeladonTeam/TotalScore"))->set_text(String(initialScore));
	static_cast<Label*>(get_node("CrimsonTeam/TotalScore"))->set_text(String(initialScore));
}

void Scoreboard::add(int64_t playerNodeName, String nickname, int64_t kills, int64_t deaths, int64_t team, int64_t role)
{
	Dictionary newPlayerRecord;
	newPlayerRecord["Name"] = nickname;
	newPlayerRecord["Kills"] = kills;
	newPlayerRecord["Deaths"] = deaths;
	newPlayerRecord["Team"] = team;
	newPlayerRecord["Role"] = role;
	scores_[playerNodeName] = newPlayerRecord;
	createScoreboardRecord(playerNodeName);
}

void Scoreboard::remove(int64_t playerNodeName)
{
	scores_.erase(playerNodeName);
	scoreboardRecords_[playerNodeName]->queue_free();
	scoreboardRecords_.erase(playerNodeName);
}

void Scoreboard::incrementKillCount(int64_t playerNodeName)
{
	Dictionary playerRecord = scores_[playerNodeName];
	playerRecord["Kills"] = static_cast<int64_t>(playerRecord["Kills"]) + 1;
	if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CELADON))
		rset("celadonTeamScore_", ++celadonTeamScore_);
	else if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CRIMSON))
		rset("crimsonTeamScore_", ++crimsonTeamScore_);
	rpc("updatePlayerScore", playerNodeName, playerRecord["Kills"], playerRecord["Deaths"]);
}

void Scoreboard::incrementDeathCount(int64_t playerNodeName)
{
	Dictionary playerRecord = scores_[playerNodeName];
	playerRecord["Deaths"] = static_cast<int64_t>(playerRecord["Deaths"]) + 1;
	rpc("updatePlayerScore", playerNodeName, playerRecord["Kills"], playerRecord["Deaths"]);
}

void Scoreboard::decrementKillCount(int64_t playerNodeName)
{
	Dictionary playerRecord = scores_[playerNodeName];
	playerRecord["Kills"] = static_cast<int64_t>(playerRecord["Kills"]) - 1;
	if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CELADON))
		rset("celadonTeamScore_", --celadonTeamScore_);
	else if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CRIMSON))
		rset("crimsonTeamScore_", --crimsonTeamScore_);
	rpc("updatePlayerScore", playerNodeName, playerRecord["Kills"], playerRecord["Deaths"]);
}

void Scoreboard::createScoreboardRecord(int64_t playerNodeName)
{
	Dictionary playerRecord = scores_[playerNodeName];
	VBoxContainer* scoresContainer;
	HBoxContainer* newRecord;
	if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CELADON))
	{
		scoresContainer = static_cast<VBoxContainer*>(get_node("CeladonTeam/Scores"));
		newRecord = static_cast<HBoxContainer*>(celadonRecordScene_->instance());
	}
	else if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CRIMSON))
	{
		scoresContainer = static_cast<VBoxContainer*>(get_node("CrimsonTeam/Scores"));
		newRecord = static_cast<HBoxContainer*>(crimsonRecordScene_->instance());
	}
	else
	{
		Godot::print("[SCOREBOARD] Error. Such team does not exist.");
		return;
	}

	static_cast<Label*>(newRecord->get_node("Name"))->set_text(String(playerRecord["Name"]));
	static_cast<Label*>(newRecord->get_node("Kills"))->set_text(String(playerRecord["Kills"]));
	static_cast<Label*>(newRecord->get_node("Deaths"))->set_text(String(playerRecord["Deaths"]));

	if (static_cast<int64_t>(playerRecord["Role"]) == static_cast<int64_t>(RoleType::WARRIOR))
	{
		static_cast<Label*>(newRecord->get_node("Role"))->set_text(String("Warrior"));
	}
	else if (static_cast<int64_t>(playerRecord["Role"]) == static_cast<int64_t>(RoleType::ARCHER))
	{
		static_cast<Label*>(newRecord->get_node("Role"))->set_text(String("Archer"));
	}
	else
	{
		Godot::print("[SCOREBOARD] Error. Such role does not exist.");
		return;
	}

	scoresContainer->add_child(newRecord);
	scoreboardRecords_.insert(pair<int64_t, HBoxContainer*>(playerNodeName, newRecord));
}

void Scoreboard::updatePlayerScore(int64_t playerNodeName, int64_t newKillCount, int64_t newDeathCount)
{
	Dictionary playerRecord = scores_[playerNodeName];
	playerRecord["Kills"] = newKillCount;
	playerRecord["Deaths"] = newDeathCount;
	updateScoreboardRecord(playerNodeName);
}

void Scoreboard::updateScoreboardRecord(int64_t playerNodeName)
{
	Dictionary playerRecord = scores_[playerNodeName];
	static_cast<Label*>(static_cast<HBoxContainer*>(scoreboardRecords_[playerNodeName])->get_node("Kills"))->set_text(String(playerRecord["Kills"]));
	static_cast<Label*>(static_cast<HBoxContainer*>(scoreboardRecords_[playerNodeName])->get_node("Deaths"))->set_text(String(playerRecord["Deaths"]));
	if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CELADON))
	{
		Variant newScore = celadonTeamScore_;
		static_cast<Label*>(get_node("CeladonTeam/TotalScore"))->set_text(String(newScore));
	}
	else if (static_cast<int64_t>(playerRecord["Team"]) == static_cast<int64_t>(Team::CRIMSON))
	{
		Variant newScore = crimsonTeamScore_;
		static_cast<Label*>(get_node("CrimsonTeam/TotalScore"))->set_text(String(newScore));
	}
	else
	{
		Godot::print("[SCOREBOARD] Error. Such team does not exist.");
	}
}
