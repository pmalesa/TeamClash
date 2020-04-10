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
	class World : public WorldEnvironment
	{
		GODOT_CLASS(World, WorldEnvironment)

	public:
		static void _register_methods();

		World() = default;
		~World() = default;

		void _init();
		void _ready();
		void generateMap();

	private:
		void placeBlock(BlockType blockType, Vector2 position);

		Dictionary blocks_;

		const int blockSize_ = 32;
		const int amplitude_ = 8;
		const int stretch_ = 10;
		const int worldLengthInBlocks_ = 500;
		const int worldSurfaceLevel_ = 640;
		const double degToRadCoefficient = Math_PI / 180;
		const int worldDepth_ = 30;
		Ref<PackedScene> blockScene_;
	};
}

/*
	With huge world dimensions (huge number of blocks) we are getting the following error message:

	"ERROR: Message queue out of memory. Try increasing 'message_queue_size_kb' in project settings.
	At: core/message_queue.cpp:56"

	It can be worked around by changing the 'message_queue_size_kb' in godot project settings.
*/