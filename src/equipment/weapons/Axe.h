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

		void _init() override;
		void _ready() override;

		void playDrawSound() override;
		void playAttackSound() override;

	private:
		void _physics_process(float delta) override;
		void _process(float delta) override;
    };
}
