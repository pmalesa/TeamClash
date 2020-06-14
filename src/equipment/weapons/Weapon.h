#pragma once

#include <Godot.hpp>
#include <Area2D.hpp>
#include <Node2D.hpp>


namespace godot
{
	class Player;

	enum class WeaponType : int64_t { NONE, DAGGER, AXE, CROSSBOW };
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


		Player* getOwner() const { return owner_; }
		Vector2 getOwnerPosition() const { return static_cast<Node2D*>(get_parent()->get_parent())->get_position(); }
		WeaponType getWeaponType() const { return weaponType_; }
		WeaponState getWeaponState() const { return weaponState_; }
		int64_t getDamage() const { return damage_; }

		void setOwner(Player* newOwner) { owner_ = newOwner; }
		void setWeaponType(WeaponType weaponType) { weaponType_ = weaponType; }
		void setWeaponState(WeaponState weaponState) { weaponState_ = weaponState; }
		void setDamage(int64_t newDamage) { damage_ = newDamage; }
		bool isRanged() const { return weaponType_ == WeaponType::CROSSBOW; }

		virtual void playDrawSound() {};
		virtual void playAttackSound() {};

	private:
		virtual void _physics_process(float delta) {};
		virtual void _process(float delta) {};

		Player* owner_;
		WeaponType weaponType_;
		WeaponState weaponState_;
		int64_t damage_;
    };
}
