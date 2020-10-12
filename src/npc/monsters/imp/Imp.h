#pragma once

#include <Godot.hpp>

#include "../Monster.h"

namespace godot
{
	class Imp : public Monster
	 {
 		GODOT_CLASS(Imp, Monster)
 
	 public:
 		static void _register_methods();
 
 		Imp() = default;
 		~Imp() = default;
 
 		void _init() override;
 		void _ready() override;
 		void _die(Player* killer) override;
 		void init(Vector2 newSpawnPoint);
 
	 private:
 		void _physics_process(float delta) override;
 		void _process(float delta) override;

		void performAttack() override;

		void setVelocity(Vector2 velocity) { velocity_ = velocity; }
 
 		void showEntanglementEffect() override;
 		void hideEntanglementEffect() override;
 		void updateSprite() override;
 
 		void _on_SlowTimer_timeout();
 		void _on_ImmobilizeTimer_timeout();
 		void _on_RespawnTimer_timeout() override;
		void _on_AIProcessingTimer_timeout();
 
 		void updateHealthPoints(int64_t newHealthPoints) override;
 		void updateHealthBar() override;
 		void updateMovementSpeed(int64_t newMovementSpeed);

		void setSlowTime(int64_t slowTime) override;
		void setImmobilizeTime(int64_t immobilizeTime) override;
		void setImmobilize(bool value) override;

		void throwback(Vector2 direction, int64_t throwbackPower) override;

		void playBodyHitSound() override;

 		static const int64_t DEFAULT_MOVEMENT_SPEED = 200;
 		static const int64_t MAX_HP = 50;
	};
}