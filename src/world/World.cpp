#include "World.h"
#include "../world/Block.h"

#include <cmath>

#include <ResourceLoader.hpp>
#include <Sprite.hpp>
#include <Texture.hpp>
#include <Shape.hpp>
#include <CollisionShape2D.hpp>
#include <RectangleShape2D.hpp>
#include <Node2D.hpp>
#include <TextureRect.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void World::_register_methods()
{
	register_method("_init", &World::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &World::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getCeladonTeamSpawnPoint", &World::getCeladonTeamSpawnPoint, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("getCrimsonTeamSpawnPoint", &World::getCrimsonTeamSpawnPoint, GODOT_METHOD_RPC_MODE_DISABLED);
}

void World::_init()
{
	resourceLoader_ = ResourceLoader::get_singleton();
	blockScene_ = resourceLoader_->load("res://world/Block.tscn");
	Godot::print("World initialised.");
}

void World::_ready()
{
	generateMap();
	generateTeamBases();
	Godot::print("World is ready.");
}

void World::generateMap()
{
	Godot::print("Generating map...");
	for (unsigned int i = 0; i < worldLengthInBlocks_; ++i)
	{
		int x_coordinate = i * blockSize_;
		int y_coordinate = 0;
		float vertical_f = amplitude_ * cos((pow(stretch_, -1) * (double)x_coordinate) * degToRadCoefficient) * blockSize_;
		int vertical_i = int(vertical_f);
		int remainder = vertical_i % blockSize_;
		if (remainder != 0)
		{
			if (remainder < blockSize_ / 2)
				vertical_i -= remainder;
			else
				vertical_i += (blockSize_ - remainder);
		}

		y_coordinate = vertical_i + worldSurfaceLevel_;
		placeBlock(BlockType::GRASS, Vector2(x_coordinate, y_coordinate));

		for (unsigned int j = 1; j * blockSize_ + y_coordinate < worldDepth_ * blockSize_; ++j)
		{
			placeBlock(BlockType::DIRT, Vector2(x_coordinate, j * 32 + y_coordinate));
		}
		placeBlock(BlockType::BEDROCK, Vector2(x_coordinate, worldDepth_ * blockSize_));
	}
	Godot::print("Map generation completed.");
}

void World::generateTeamBases()
{
	createBase(Team::CELADON, 30 * blockSize_, BaseOrientation::RIGHTWARDS);
	createBase(Team::CRIMSON, (worldLengthInBlocks_ - 31) * blockSize_, BaseOrientation::LEFTWARDS);
}

void World::createBase(Team team, real_t startingCoordinateX, BaseOrientation baseOrientation)
{
	if (startingCoordinateX < 0 || startingCoordinateX > worldLengthInBlocks_ * blockSize_)
	{
		Godot::print("[WORLD] Error occurred while generating the base. Starting coordinate was too large or too low.");
		return;
	}

	/* Finding starting point */
	Vector2 utmostBlockPosition = Vector2(startingCoordinateX, worldSurfaceLevel_);
	if (blocks_.has(utmostBlockPosition))
	{
		while (blocks_.has(utmostBlockPosition))
			utmostBlockPosition -= Vector2(0, blockSize_);
	}
	else
	{
		while (!blocks_.has(utmostBlockPosition))
			utmostBlockPosition += Vector2(0, blockSize_);
		utmostBlockPosition -= Vector2(0, blockSize_);
	}
	real_t maxTerrainLevel = utmostBlockPosition.y;

	bool baseGeneratedCorrectly = false;
	if (baseOrientation == BaseOrientation::RIGHTWARDS)
	{
		if (startingCoordinateX + baseWidthInBlocks_ * blockSize_ > worldLengthInBlocks_ * blockSize_)
		{
			Godot::print("[WORLD] Error occurred while generating the base. Base can not stick out of the map itself.");
			return;
		}

		/* Finding maximum terrain height */
		Vector2 currentPosition = utmostBlockPosition;
		for (unsigned int i = 0; i < baseWidthInBlocks_; ++i)
		{
			while (blocks_.has(currentPosition))
			{
				currentPosition -= Vector2(0, blockSize_);
				if (currentPosition.y < maxTerrainLevel)
					maxTerrainLevel = currentPosition.y;
			}
			currentPosition += Vector2(blockSize_, 0);
		}
		/* Filling the floor of the base */
		for (unsigned int i = 0; i < baseWidthInBlocks_; ++i)
		{
			currentPosition = Vector2(utmostBlockPosition.x + i * blockSize_, maxTerrainLevel);
			while (!blocks_.has(currentPosition))
			{
				placeBlock(BlockType::MARBLE, currentPosition);
				currentPosition += Vector2(0, blockSize_);
			}
		}

		/* Creating the back wall of the base */
		currentPosition = Vector2(utmostBlockPosition.x, maxTerrainLevel - blockSize_);
		for (unsigned int i = 0; i < 15; ++i)
			placeBlock(BlockType::MARBLE, Vector2(currentPosition.x, currentPosition.y - i * blockSize_));

		baseGeneratedCorrectly = true;
	}
	else if (baseOrientation == BaseOrientation::LEFTWARDS)
	{
		if (startingCoordinateX - baseWidthInBlocks_ * blockSize_ < 0)
		{
			Godot::print("[WORLD] Error occurred while generating the base. Base can not stick out of the map itself.");
			return;
		}

		/* Finding maximum terrain height */
		Vector2 currentPosition = utmostBlockPosition;
		for (unsigned int i = 0; i < baseWidthInBlocks_; ++i)
		{
			while (blocks_.has(currentPosition))
			{
				currentPosition -= Vector2(0, blockSize_);
				if (currentPosition.y < maxTerrainLevel)
					maxTerrainLevel = currentPosition.y;
			}
			currentPosition -= Vector2(blockSize_, 0);
		}
		/* Filling the floor of the base */
		for (unsigned int i = 0; i < baseWidthInBlocks_; ++i)
		{
			currentPosition = Vector2(utmostBlockPosition.x - i * blockSize_, maxTerrainLevel);
			while (!blocks_.has(currentPosition))
			{
				placeBlock(BlockType::MARBLE, currentPosition);
				currentPosition += Vector2(0, blockSize_);
			}
		}

		/* Creating the back wall of the base */
		currentPosition = Vector2(utmostBlockPosition.x, maxTerrainLevel - blockSize_);
		for (unsigned int i = 0; i < 10; ++i)
			placeBlock(BlockType::MARBLE, Vector2(currentPosition.x, currentPosition.y - i * blockSize_));

		baseGeneratedCorrectly = true;
	}
	else
		Godot::print("[WORLD] Error occurred while generating the base. Base orientation was not specified correctly.");

	/* Placing the team's flag */
	if (baseGeneratedCorrectly)
	{
		if (team == Team::CELADON)
		{
			if (baseOrientation == BaseOrientation::RIGHTWARDS)
				static_cast<Node2D*>(get_node("CeladonFlag"))->set_position(Vector2(utmostBlockPosition.x + blockSize_ * (baseWidthInBlocks_ / 2), maxTerrainLevel - blockSize_));
			else
				static_cast<Node2D*>(get_node("CeladonFlag"))->set_position(Vector2(utmostBlockPosition.x - blockSize_ * (baseWidthInBlocks_ / 2), maxTerrainLevel - blockSize_));
			celadonTeamSpawnPoint_ = static_cast<Node2D*>(get_node("CeladonFlag"))->get_position();
		}
		else if (team == Team::CRIMSON)
		{
			if (baseOrientation == BaseOrientation::RIGHTWARDS)
				static_cast<Node2D*>(get_node("CrimsonFlag"))->set_position(Vector2(utmostBlockPosition.x + blockSize_ * (baseWidthInBlocks_ / 2), maxTerrainLevel - blockSize_));
			else
				static_cast<Node2D*>(get_node("CrimsonFlag"))->set_position(Vector2(utmostBlockPosition.x - blockSize_ * (baseWidthInBlocks_ / 2), maxTerrainLevel - blockSize_));
			crimsonTeamSpawnPoint_ = static_cast<Node2D*>(get_node("CrimsonFlag"))->get_position();
		}
		else
			Godot::print("[WORLD] Error occurred while placing team flag. Team was not chosen correctly.");
	}
}

void World::placeBlock(BlockType blockType, Vector2 position)
{
	if (int64_t(position.x) % blockSize_ == 0 && int64_t(position.y) % blockSize_ == 0 && !blocks_.has(position))
	{
		position = Vector2(int64_t(position.x), int64_t(position.y));
		Block* newBlock = static_cast<Block*>(blockScene_->instance());
		newBlock->init(blockType, position);
		blocks_[position] = newBlock;
		add_child(newBlock);
	}
}