#pragma once

#include <Godot.hpp>
#include <StaticBody2D.hpp>
#include <Array.hpp>
#include <Vector2.hpp>

namespace godot
{
	enum class BlockType
	{
		BEDROCK,
		GRASS,
		DIRT
	};

	class Block : public StaticBody2D
	{
		GODOT_CLASS(Block, StaticBody2D)
	
	public:
		static void _register_methods();

		Block() = default;
		~Block() = default;

		void _init();
		void init(BlockType blockType, Vector2 position);
		void _ready();
		
	private:
		BlockType blockType_;
		Vector2 position_;
	};
}