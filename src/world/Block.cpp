#include "Block.h"

#include <ResourceLoader.hpp>
#include <Sprite.hpp>
#include <Texture.hpp>

using namespace godot;

void Block::_register_methods()
{
	register_method("_init", &Block::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &Block::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Block::_init()
{

}

void Block::init(BlockType blockType, Vector2 position)
{
	ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	godot::Sprite* sprite = static_cast<godot::Sprite*>(get_node("Sprite"));
	
	blockType_ = blockType;

	/* THIS CRASHES THE GAME, fixing this may speed up map generation */
	//position_ = position;

	if (blockType == BlockType::GRASS)
		sprite->set_texture(resourceLoader->load("res://sprites/blocks/Grass.png"));

	else if (blockType == BlockType::DIRT)
		sprite->set_texture(resourceLoader->load("res://sprites/blocks/Dirt.png"));

	else if (blockType == BlockType::BEDROCK)
		sprite->set_texture(resourceLoader->load("res://sprites/blocks/Bedrock.png"));
	set_position(position);
}

void Block::_ready()
{
	
}