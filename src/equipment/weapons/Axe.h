#pragma once
#include "Weapon.h"
#include <Godot.hpp>

namespace godot
{
    class Axe : public Weapon
    {
        GODOT_CLASS(Axe, Weapon)

    public:
		static void _register_methods();

		Axe() = default;
		~Axe() = default;

		void _init();
		void _ready();

		void playDrawSound();
		void playAttackSound();

	private:
		void _physics_process(float delta);
		void _process(float delta);

    };
}
