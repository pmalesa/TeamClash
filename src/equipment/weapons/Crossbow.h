#pragma once
#include "Weapon.h"

#include <Godot.hpp>
#include <Timer.hpp>

namespace godot
{
	class PackedScene;
	class Bolt;

	enum class ProjectileType : int64_t { BOLT, EXPLOSIVE_BOLT };

	class Crossbow : public Weapon
	{
		GODOT_CLASS(Crossbow, Weapon)

	public:
		static void _register_methods();

		Crossbow() = default;
		~Crossbow() = default;

		void _init();
		void _ready();

		void setAmmoType(ProjectileType ammoType) { currentAmmoType_ = ammoType; }
		ProjectileType getAmmoType() const { return currentAmmoType_; }

		void playDrawSound();
		void playAttackSound();

		bool boltOnCooldown() { return static_cast<Timer*>(get_node("BoltCooldown"))->get_time_left() > 0; }
		bool explosiveBoltOnCooldown() { return static_cast<Timer*>(get_node("ExplosiveBoltCooldown"))->get_time_left() > 0; }

	private:
		void _physics_process(float delta);
		void _process(float delta);
		void processRangedAttack();

		void _on_BoltCooldown_timeout() { static_cast<Timer*>(get_node("BoltCooldown"))->stop(); };
		void _on_ExplosiveBoltCooldown_timeout() { static_cast<Timer*>(get_node("ExplosiveBoltCooldown"))->stop(); };

		void activateBolt(String boltNodeName, String shooterNodeName, Vector2 initialPosition, Vector2 initialDirection);
		void activateExplosiveBolt(String boltNodeName, String shooterNodeName, Vector2 initialPosition, Vector2 initialDirection);

		void shootBolt();
		void shootExplosiveBolt();

		ProjectileType currentAmmoType_;

		Ref<PackedScene> boltScene_;
		Ref<PackedScene> explosiveBoltScene_;
	};
}
