#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

#include "../../player/Player.h"
#include "../../player/ui/HealthBar.h"
#include <Sprite.hpp>
#include <Timer.hpp>

#include <random>
#include <ctime>

namespace godot
{
	class Label;
	class AnimationPlayer;
	class ResourceLoader;
	class PackedScene;
	class Control;
	class CanvasLayer;

	enum class AIState : int64_t { SEEKING, ENGAGING, FLEEING};

	class Monster : public KinematicBody2D
	{
		GODOT_CLASS(Monster, KinematicBody2D)

	public:
		static void _register_methods();

		Monster() = default;
		virtual ~Monster() = default;

		virtual void _init();
		virtual void _ready();
		void _move(int64_t direction);

		//void _on_AIProcessingTimer_timeout();

		void processMovement(); // TODO
		void acquireTarget(); // TODO

		Vector2 getVelocity() const { return velocity_; }
		int64_t getHealthPoints() const { return healthPoints_; }
		HealthBar* getHealthBar() const { return healthBar_; }
		int64_t getNodeName() const { return nodeName_; }
		Vector2 getSpawnPoint() const { return spawnPoint_; }
		bool isInitialized() const { return initialized_; }
		bool isImmobilized() const { return immobilized_; }

		void setVelocity(Vector2 velocity) { velocity_ = velocity; }
		void addVelocity(Vector2 velocity) { velocity_ += velocity; }
		void setHealthPoints(int64_t newHealthPoints) { healthPoints_ = newHealthPoints; }
		void setNodeName(int64_t nodeName) { nodeName_ = nodeName; }
		void setInitialized(bool val) { initialized_ = val; }
		void setCurrentMovementSpeed(int64_t newMovementSpeed) { currentMovementSpeed_ = newMovementSpeed; }

		void setSpawnPoint(Vector2 newSpawnPoint) { spawnPoint_ = newSpawnPoint; }

		void inflictDamage(int64_t damage, Player* attacker = nullptr);
		void inflictSlow(int64_t newSpeed, int64_t slowTime);
		void immobilize(int64_t immobilizeTime);
		void applyThrowback(Vector2 direction, int64_t throwbackPower = DEFAULT_THROWBACK_POWER);

	protected:
		int64_t healthPoints_;
		int64_t nodeName_;

		Timer* AIProcessingTimer_;

		Player* target_;
		AIState currentAIState_;

		Vector2 velocity_;
		MoveDirection moveDirection_;
		MovementState movementState_;
		bool immobilized_;

		Vector2 slavePosition;
		int64_t slaveMovement;
		int64_t slaveWeaponState;

		Vector2 throwbackVelocity_;
		bool applyThrowback_;
		Vector2 facingDirection_;

	private:
		virtual void _process(float delta) { }
		virtual void _physics_process(float delta) { }

		void jump();
		virtual void performAttack() { }

		virtual void _die(Player* killer) { };
		virtual void _on_RespawnTimer_timeout() { }

		virtual void showEntanglementEffect() { }
		virtual void hideEntanglementEffect() { }
		virtual void updateSprite() { };

		virtual void updateHealthPoints(int64_t newHealthPoints) { }
		virtual void updateHealthBar() { }
		virtual void updateMovementSpeed(int64_t newMovementSpeed) { }

		virtual void setSlowTime(int64_t slowTime) { }
		virtual void setImmobilizeTime(int64_t immobilizeTime) { }
		virtual void setImmobilize(bool value) { }

		virtual void throwback(Vector2 direction, int64_t throwbackPower) { }

		virtual void playBodyHitSound() { }

		const float JUMP_POWER = 350.0f;
		const float GRAVITY_PULL = 20.0f;
		const static int64_t DEFAULT_THROWBACK_POWER = 400;
		const float HORIZONTAL_THROWBACK_DECAY = 30.0f;

		bool initialized_;

		Vector2 spawnPoint_;
		int64_t currentMovementSpeed_;

		HealthBar* healthBar_;
		float damageFactor_;

		ResourceLoader* resourceLoader_;
	};
}