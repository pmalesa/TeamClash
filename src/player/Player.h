#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

#include "../equipment/Weapon.h"
#include "HealthBar.h"

namespace godot
{
	class Label;
	class AnimationPlayer;
	class ResourceLoader;
	class PackedScene;

    enum class MoveDirection : int64_t { LEFT, RIGHT, NONE };
	enum class MovementState : int64_t { NONE, JUMPED, FALLING, THROWBACK };

    class Player : public KinematicBody2D
    {
        GODOT_CLASS(Player, KinematicBody2D)

    public:
        static void _register_methods();

        Player() = default;
        ~Player() = default;

        void _init();
        void _ready();
        void init(String nickname, Vector2 startPosition, bool isSlave);

		String getNickname() const { return nickname_; }
		Vector2 getVelocity() const { return velocity_; }
		int64_t getHealthPoints() const { return healthPoints_; }
		int64_t getNodeName() const { return nodeName_; }

		void setNickname(String newNickname) { nickname_ = newNickname; }
		void setVelocity(Vector2 velocity) { velocity_ = velocity; }
		void addVelocity(Vector2 velocity) { velocity_ += velocity; }

		void inflictDamage(int64_t damage);
		void throwback(Vector2 direction);

    private:
		void _physics_process(float delta);
		void _process(float delta);
		void _move(int64_t direction);
		void _die();
		void _on_RespawnTimer_timeout();

		void processMeleeAttack();
		void processRangedAttack();
		void shootBolt();
		void updateInput();
		void updateSprite();
		void updateHealthPoints(int64_t newHealthPoints);
		void updateHealthBar();
		void updateArmRotation(Vector2 aimingDirection);
		void setWeaponTo(int64_t weaponType);
		void updateAimingDirection();
		void playBodyHitSound();

        const float MOVE_SPEED = 300.0f;
        int64_t MAX_HP = 100;
		const float JUMP_POWER = 600.0f;
		const float GRAVITY_PULL = 20.0f;
		const float THROWBACK_POWER = 400.0f;
		const float HORIZONTAL_THROWBACK_DECAY = 30.0f;

		String nickname_;
		Label* nicknameLabel_;
		Vector2 velocity_;
		Vector2 facingDirection_;
		Vector2 aimingDirection_;
		Weapon* currentWeapon_;
		Ref<PackedScene> boltScene_;

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

		ResourceLoader* resourceLoader_;
		Ref<PackedScene> weaponScene_;
    };

}
