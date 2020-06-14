#pragma once
#include "Weapon.h"
#include <Godot.hpp>

namespace godot
{
    class Dagger : public Weapon
    {
        GODOT_CLASS(Dagger, Weapon)

    public:
        static void _register_methods();

        Dagger() = default;
        ~Dagger() = default;

        void _init();
        void _ready();

		void playDrawSound();
		void playAttackSound();

	private:
		void _physics_process(float delta);
		void _process(float delta);

		Array alreadyAttackedPlayers_;
    };
}
