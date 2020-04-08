#pragma once

#include <Godot.hpp>
#include <Area2D.hpp>

#include <Sprite.hpp>
#include <ResourceLoader.hpp>

namespace godot
{
	enum class WeaponType : int64_t { FISTS, STICK, SWORD, AXE, CLUB };
	enum class WeaponState : int64_t { IDLE, ATTACKING };

    class Weapon : public Area2D
    {
        GODOT_CLASS(Weapon, Area2D)

    public:
        static void _register_methods();

        Weapon() = default;
        ~Weapon() = default;

        void _init();
		void init(WeaponType weaponType);
        void _ready();
		void _physics_process(float delta);

		int64_t getDamage() const { return damage_; }

	private:
		int64_t damage_;

    };
}
