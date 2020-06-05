#pragma once
#include "Weapon.h"
#include <Godot.hpp>

namespace godot
{
	class Crossbow : public Weapon
	{
		GODOT_CLASS(Crossbow, Weapon)

	public:
		static void _register_methods();

		Crossbow() = default;
		~Crossbow() = default;

		void _init();
		void _ready();

		void playDrawSound();
		void playAttackSound();

	private:
		void _physics_process(float delta);
		void _process(float delta);

	};
}
