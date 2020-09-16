#pragma once
#include <core/Godot.hpp>
#include <Node2D.hpp>

namespace godot
{
	class MenuWindow : public Node2D
	{
		GODOT_CLASS(MenuWindow, Node2D)
	public:
		MenuWindow() = default;
		~MenuWindow() = default;

		void _init();
		static void _register_methods();

	private:
		void _on_ResumeButton_pressed();
		void _on_MainMenuButton_pressed();
		void _on_ExitGameButton_pressed();
	};
}