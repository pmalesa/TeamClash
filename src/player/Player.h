#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

#include "../equipment/weapons/Weapon.h"
#include "HealthBar.h"
#include <Sprite.hpp>
#include <Timer.hpp>

namespace godot
{
	class Label;
	class AnimationPlayer;
	class ResourceLoader;
	class PackedScene;
	class Control;

    enum class MoveDirection : int64_t { LEFT, RIGHT, NONE };
	enum class MovementState : int64_t { NONE, JUMPED, FALLING, THROWBACK };
	enum class Team : int64_t { CELADON, CRIMSON };
	enum class Role : int64_t { WARRIOR, ARCHER };

    class Player : public KinematicBody2D
    {
        GODOT_CLASS(Player, KinematicBody2D)

    public:
        static void _register_methods();

        Player() = default;
        ~Player() = default;

        void _init();
        void _ready();
        void init(int64_t chosenTeam, int64_t chosenRole);

		String getNickname() const { return nickname_; }
		Vector2 getVelocity() const { return velocity_; }
		int64_t getHealthPoints() const { return healthPoints_; }
		int64_t getNodeName() const { return nodeName_; }

		void setNickname(String newNickname) { nickname_ = newNickname; }
		void setVelocity(Vector2 velocity) { velocity_ = velocity; }
		void addVelocity(Vector2 velocity) { velocity_ += velocity; }

		void inflictDamage(int64_t damage);
		void inflictSlow(int64_t slowAmount, int64_t slowTime);
		void throwback(Vector2 direction, int64_t throwbackPower = THROWBACK_POWER);

    private:
		void _physics_process(float delta);
		void _process(float delta);
		void _move(int64_t direction);
		void _die();
		void _on_SlowTimer_timeout();
		void _on_RespawnTimer_timeout();
		void _on_BoltCooldown_timeout();
		void _on_ExplosiveBoltCooldown_timeout();
		void _on_EntanglingBallsCooldown_timeout();

		void setTeam(int64_t team);
		void setRole(int64_t role);
		void setUI(int64_t role);
		void setSpawnPoint(Vector2 newSpawnPoint) { spawnPoint_ = newSpawnPoint; }

		void processMeleeAttack();
		void processRangedAttack();
		void processThrow();
		void shootBolt();
		void throwEntanglingBalls();
		void showEntanglementEffect() { static_cast<Sprite*>(get_node("body_sprite/Entanglement"))->set_visible(true); }
		void hideEntanglementEffect() { static_cast<Sprite*>(get_node("body_sprite/Entanglement"))->set_visible(false); }
		void updateInput();
		void updateSprite();
		void updateHealthPoints(int64_t newHealthPoints);
		void updateHealthBar();
		void updateMovementSpeed(int64_t newMovementSpeed);
		void setSlowTime(int64_t slowTime);
		void updateArmRotation(Vector2 aimingDirection);
		void switchWeapon();
		void setProjectileTypeTo(int64_t newProjectileType);
		void updateAimingDirection();
		void playBodyHitSound();

		bool boltOnCooldown() { return static_cast<Timer*>(get_node("BoltCooldown"))->get_time_left() > 0; }
		bool explosiveBoltOnCooldown() { return static_cast<Timer*>(get_node("ExplosiveBoltCooldown"))->get_time_left() > 0; }
		bool entanglingBallsOnCooldown() { return static_cast<Timer*>(get_node("EntanglingBallsCooldown"))->get_time_left() > 0; }

        static const int64_t DEFAULT_MOVEMENT_SPEED = 300;
        static const int64_t MAX_HP = 100;
		const float JUMP_POWER = 600.0f;
		const float GRAVITY_PULL = 20.0f;
		const static int64_t THROWBACK_POWER = 400;
		const float HORIZONTAL_THROWBACK_DECAY = 30.0f;

		bool initialized_;

		Control* ui_;
		String nickname_;
		int64_t role_;
		Label* nicknameLabel_;
		Vector2 velocity_;
		Vector2 facingDirection_;
		Vector2 aimingDirection_;
		Vector2 spawnPoint_;
		String animationNameSuffix_;
		Array weapons_;
		Weapon* currentWeapon_;
		ProjectileType currentAmmoType_;
		int64_t currentMovementSpeed_;

		MoveDirection moveDirection_;
		MovementState movementState_;
        WeaponState weaponState_;

        Vector2 slavePosition;
        int64_t slaveMovement;
        int64_t slaveWeaponState;

        int64_t healthPoints_;
		HealthBar* healthBar_;
        int64_t nodeName_;

		Array alreadyAttackedPlayers_;
		Vector2 throwbackVelocity_;
		bool applyThrowback_;
		bool entanglingBallsThrown_;

		ResourceLoader* resourceLoader_;
		Ref<PackedScene> weaponScene_;
		Ref<PackedScene> boltScene_;
		Ref<PackedScene> explosiveBoltScene_;
		Ref<PackedScene> entanglingBallsScene_;
    };

}
