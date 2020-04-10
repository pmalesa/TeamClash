#include "World.h"
#include "../world/Block.h"

#include <cmath>

#include <ResourceLoader.hpp>
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
	ResourceLoader* resourceLoader_ = ResourceLoader::get_singleton();
	blockScene_ = resourceLoader_->load("res://world/Block.tscn"); // This line causes the ERROR: "...does not have a library for current platform..."
	Godot::print("World initialised.");
}

void World::_ready()
{
	generateMap();
	Godot::print("World is ready.");
}

void World::generateMap()
{
	Godot::print("Generating map...");
	for (unsigned int i = 0; i < worldLengthInBlocks_; ++i)
	{
		int x_coordinate = i * blockSize_;
		int y_coordinate = 0;
		float vertical = amplitude_ * cos((pow(stretch_, -1) * (double)x_coordinate) * degToRadCoefficient) * blockSize_;
		int remainder = int(vertical) % blockSize_;
		if (remainder != 0)
		{
			if (remainder < blockSize_ / 2)
				vertical -= remainder;
			else
				vertical += (blockSize_ - remainder);
		}

		y_coordinate = vertical + worldSurfaceLevel_;
		placeBlock(BlockType::GRASS, Vector2(x_coordinate, y_coordinate));

		for (unsigned int j = 1; j * blockSize_ + y_coordinate < worldDepth_ * blockSize_; ++j)
		{
			placeBlock(BlockType::DIRT, Vector2(x_coordinate, j * 32 + y_coordinate));
		}
		placeBlock(BlockType::BEDROCK, Vector2(x_coordinate, worldDepth_ * blockSize_));
	}
	Godot::print("Map generation completed.");
}

void World::placeBlock(BlockType blockType, Vector2 position)
{
	if (!blocks_.has(position))
	{
		Block* newBlock = static_cast<Block*>(blockScene_->instance());
		newBlock->init(blockType, position);
		blocks_[position] = newBlock;
		add_child(newBlock);
	}
}