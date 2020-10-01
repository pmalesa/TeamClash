#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>
#include "../../player/Player.h"

#include <iostream>

namespace godot
{
    class Bolt : public KinematicBody2D
    {
        GODOT_CLASS(Bolt, KinematicBody2D)

    public:
        static void _register_methods();

		Bolt() = default;
        ~Bolt() = default;

        void _init();
		void activate(Player* shooter, Vector2 initialPosition, Vector2 initialDirection);

	private:
		void _physics_process(float delta);
		void _process(float delta);
		void processImpact();
		void processMovement();
		void _on_BoltLifeAfterHitTimer_timeout();
		void _on_BoltMaximumLifeTimer_timeout();

		void deactivate();
		bool collisionDetected();

		const static int64_t damage_ = 35;
		const static int64_t INITIAL_PROJECTILE_SPEED = 1700;
		const static int64_t GRAVITY_PULL = 20;
		const static int64_t AIR_RESISTANCE = 2;
		const static int64_t MAX_FALLING_SPEED = 1800;

		Vector2 velocity_;
		Vector2 initialDirection_;
		Player* shooter_;

		Vector2 slavePosition_;
		real_t slaveRotation_;

		bool activated_;
		bool objectHit_;

    };
}
