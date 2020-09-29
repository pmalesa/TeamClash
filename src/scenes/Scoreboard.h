#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>

#include <unordered_map>

#include <PackedScene.hpp>
#include <HBoxContainer.hpp>

using std::unordered_map;

namespace godot
{
	class Scoreboard : public Node2D
	{
		GODOT_CLASS(Scoreboard, Node2D)
	public:
		Scoreboard() = default;
		~Scoreboard() = default;

		static void _register_methods();
		void _init();
		void _ready();

		void show() { set_visible(true); }
		void hide() { set_visible(false); }

		void add(int64_t playerNodeName, String nickname, int64_t kills, int64_t deaths, int64_t team, int64_t role);
		void remove(int64_t playerNodeName);

		void incrementKillCount(int64_t playerNodeName);
		void incrementDeathCount(int64_t playerNodeName);
		void decrementKillCount(int64_t playerNodeName);

	private:
		void createScoreboardRecord(int64_t playerNodeName);
		void updatePlayerScore(int64_t playerNodeName, int64_t newKillCount, int64_t newDeathCount);
		void updateScoreboardRecord(int64_t playerNodeName);

		Ref<PackedScene> celadonRecordScene_;
		Ref<PackedScene> crimsonRecordScene_;

		Dictionary scores_;
		unordered_map<int64_t, HBoxContainer*> scoreboardRecords_;

		int64_t celadonTeamScore_;
		int64_t crimsonTeamScore_;
	};
}