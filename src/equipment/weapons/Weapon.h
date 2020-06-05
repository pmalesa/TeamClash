#pragma once

#include <Godot.hpp>
#include <Area2D.hpp>

namespace godot
{
	enum class WeaponType : int64_t { NONE, DAGGER, AXE, CROSSBOW };
	enum class ProjectileType : int64_t { BOLT, EXPLOSIVE_BOLT };
	enum class WeaponState : int64_t { IDLE, ATTACKING, SHOOTING, THROWING };

    class Weapon : public Area2D
    {
        GODOT_CLASS(Weapon, Area2D)

    public:
        static void _register_methods();

        Weapon() = default;
        virtual ~Weapon() = default;

		virtual void _init() {};
		virtual void _ready() {};

		int64_t getWeaponType() const { return weaponType_; }
		int64_t getDamage() const { return damage_; }
		bool isRanged() const { return weaponType_ == static_cast<int64_t>(WeaponType::CROSSBOW); }

		virtual void playDrawSound() {};
		virtual void playAttackSound() {};

	protected:
		virtual void _physics_process(float delta) {};
		virtual void _process(float delta) {};

		int64_t weaponType_;
		int64_t damage_;
    };
}
