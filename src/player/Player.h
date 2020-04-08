#pragma once

#include <memory>
#include <Godot.hpp>
#include <KinematicBody2D.hpp>

#include <PackedScene.hpp>
#include <ResourceLoader.hpp>

#include "../equipment/Weapon.h"
#include "HealthBar.h"

namespace godot
{
	class Label;

    enum class MoveDirection : int64_t { LEFT, RIGHT, NONE };
	enum class MovementState : int64_t { NONE, JUMPED, FALLING };

    class Player : public KinematicBody2D
    {
        GODOT_CLASS(Player, KinematicBody2D)

    public:
        static void _register_methods();

        Player() = default;
        ~Player() = default;

        void _init();
        void _ready();
        void _physics_process(float delta);
		void _process(float delta);
        void _move(int64_t direction);
		void _on_HealthBar_value_changed(float value);
		void _die();
		void _on_RespawnTimer_timeout();

        void inflictDamage(int64_t damage);
		void processAttack();
		void updateInput();
        void updateSprite();
        void init(String nickname, Vector2 startPosition, bool isSlave);
		void setWeapon(WeaponType weaponType);

		String getNickname() const { return nickname_; }
		Vector2 getVelocity() const { return velocity_; }
		int64_t getHealthPoints() const { return healthPoints_; }
		int64_t getNodeName() const { return nodeName_; }

		void setNickname(String newNickname) { nickname_ = newNickname; }
		void setVelocity(Vector2 velocity) { velocity_ = velocity; }
		void addVelocity(Vector2 velocity) { velocity_ += velocity; }
		void updateHealthPoints(int64_t newHealthPoints);
		void updateHealthBar();

    private:
        const float MOVE_SPEED = 300.0f;
        int64_t MAX_HP = 100;
		const float JUMP_POWER = 600.0f;
		const float GRAVITY_PULL = 20.0f;

		String nickname_;
		Label* nicknameLabel_;
		Vector2 velocity_;
		Weapon* currentWeapon_;

		MoveDirection moveDirection_;
		MovementState movementState_;
        WeaponState weaponState_;

        Vector2 slavePosition;
        int64_t slaveMovement;
        int64_t slaveWeaponState;

        int64_t healthPoints_;
		HealthBar* healthBar_;
        int64_t nodeName_;

		Ref<PackedScene> weaponScene_;
		ResourceLoader* resourceLoader_;
    };

}
