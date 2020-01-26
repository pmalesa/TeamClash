#pragma once

#include <Godot.hpp>
#include <StaticBody2D.hpp>

namespace godot
{
	class Biome : public Node2D
	{
		GODOT_CLASS(Biome, Node2D)

	public:
		static void _register_methods();

		Biome() = default;
		~Biome() = default;

		void _init();
		void _ready();
	};
}