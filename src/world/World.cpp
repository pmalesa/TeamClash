#include "World.h"
#include "../world/Block.h"

#include <cmath>

#include <PackedScene.hpp>

#include <Sprite.hpp>
#include <Texture.hpp>
#include <Shape.hpp>
#include <CollisionShape2D.hpp>
#include <RectangleShape2D.hpp>





using namespace godot;

void World::_register_methods()
{
	register_method("_init", &World::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &World::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
}

void World::_init()
{
	resourceLoader_ = ResourceLoader::get_singleton();
	Godot::print("Generating map...");
	generateMap();
	Godot::print("Map generation completed.");
	Godot::print("World initialised.");
}

void World::_ready()
{
	Godot::print("World is ready.");
}

void World::placeBlock(BlockType blockType, Vector2 position)
{
	if (blocks_.find(position) == blocks_.end())
	{
		Ref<PackedScene> blockScene_ = resourceLoader_->load("res://world/Block.tscn");
		godot::Block* newBlock = static_cast<godot::Block*>(blockScene_->instance());
		newBlock->init(blockType, position);
		blocks_.insert(std::pair<Vector2, std::unique_ptr<godot::Block>>(position, newBlock));
		add_child(newBlock);
	}
}

void World::generateMap()
{
	for (unsigned int i = 0; i < worldLengthInBlocks_; ++i)
	{
		int x_coordinate = i * blockSize_;
		int y_coordinate;
		int vertical = amplitude_ * cos((pow(stretch_, -1) * (double)x_coordinate) * degToRadCoefficient) * blockSize_;
		int remainder = vertical % blockSize_;
		if (remainder != 0)
		{
			if (remainder < blockSize_ / 2)
				vertical -= remainder;
			else
				vertical += (blockSize_ - remainder);
		}

		y_coordinate = vertical += 640;
		placeBlock(BlockType::GRASS, Vector2(x_coordinate, y_coordinate));

		for (unsigned int j = 1; j * 32 + y_coordinate < worldDepth_ * blockSize_; ++j)
		{
			placeBlock(BlockType::DIRT, Vector2(x_coordinate, j * 32 + y_coordinate));
		}
		placeBlock(BlockType::BEDROCK, Vector2(x_coordinate, worldDepth_ * blockSize_));
	}
}