#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>

namespace godot
{
	class CreateGameScene : public Node2D
	{
		GODOT_CLASS(CreateGameScene, Node2D)
	public:
		CreateGameScene() = default;
		~CreateGameScene() = default;

		static void _register_methods();
		void _init();
		void _on_StartGameButton_pressed(Variant body);


	};
}