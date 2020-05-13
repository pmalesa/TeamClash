#pragma once

#include <Godot.hpp>
#include <Area2D.hpp>

namespace godot
{
	enum class WeaponType : int64_t { NONE, FISTS, STICK, SWORD, AXE, CLUB, CROSSBOW };
	enum class ProjectileType : int64_t { BOLT, EXPLOSIVE_BOLT };
	enum class WeaponState : int64_t { IDLE, ATTACKING, SHOOTING };

    class Weapon : public Area2D
    {
        GODOT_CLASS(Weapon, Area2D)

    public:
        static void _register_methods();

        Weapon() = default;
        ~Weapon() = default;

        void _init();
        void _ready();

		void setWeapon(WeaponType weaponType);
		int64_t getDamage() const { return damage_; }
		void setDamage(int64_t newDamage) { damage_ = newDamage; }
		bool isRanged() const { return weaponType_ == WeaponType::CROSSBOW; }

	private:
		void _physics_process(float delta);
		void _process(float delta);

		WeaponType weaponType_;
		int64_t damage_;
    };
}
