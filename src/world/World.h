#pragma once

#include "Block.h"

#include <Godot.hpp>
#include <WorldEnvironment.hpp>
#include <Vector2.hpp>
#include <Dictionary.hpp>
#include <PackedScene.hpp>

#include <map>
#include <memory>

namespace godot
{
	class ResourceLoader;

	enum class BaseOrientation : int64_t { RIGHTWARDS, LEFTWARDS };
	enum class Team : int64_t { CELADON, CRIMSON };

	class World : public WorldEnvironment
	{
		GODOT_CLASS(World, WorldEnvironment)

	public:
		static void _register_methods();

		World() = default;
		~World() = default;

		void _init();
		void _ready();

		Vector2 getCeladonTeamSpawnPoint() { return celadonTeamSpawnPoint_; }
		Vector2 getCrimsonTeamSpawnPoint() { return crimsonTeamSpawnPoint_; }

	private:
		void generateMap();
		void generateTeamBases();
		void createBase(Team team, real_t startingCoordinateX = 0, BaseOrientation baseOrientation = BaseOrientation::RIGHTWARDS);
		void placeBlock(BlockType blockType, Vector2 position);

		Dictionary blocks_;

		const int blockSize_ = 32;
		const int amplitude_ = 8;
		const int stretch_ = 12;
		const int worldLengthInBlocks_ = 200;
		const int worldSurfaceLevel_ = 640;
		const double degToRadCoefficient = Math_PI / 180;
		const int worldDepth_ = 30;
		const int baseWidthInBlocks_ = 15;
		Vector2 celadonTeamSpawnPoint_;
		Vector2 crimsonTeamSpawnPoint_;

		ResourceLoader* resourceLoader_;
		Ref<PackedScene> blockScene_;
	};
}

/*
	With huge world dimensions (huge number of blocks) we are getting the following error message:

	"ERROR: Message queue out of memory. Try increasing 'message_queue_size_kb' in project settings.
	At: core/message_queue.cpp:56"

	It can be worked around by changing the 'message_queue_size_kb' in godot project settings.
*/