#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>

namespace godot
{
	class MainMenu : public Node2D
	{
		GODOT_CLASS(MainMenu, Node2D)
	public:
		MainMenu() = default;
		~MainMenu() = default;

		static void _register_methods();
		void _init();
		void _on_CreateGameButton_pressed(Variant body);


	};
}