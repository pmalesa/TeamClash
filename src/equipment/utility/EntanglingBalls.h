#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

#include <iostream>

namespace godot
{
    class EntanglingBalls : public KinematicBody2D
    {
        GODOT_CLASS(EntanglingBalls, KinematicBody2D)

    public:
        static void _register_methods();

		EntanglingBalls() = default;
        ~EntanglingBalls() = default;

        void _init();
		void activate(int64_t shooterNodeName, Vector2 initialPosition, Vector2 initialDirection);

		int64_t getSlowDuration() { return SLOW_DURATION; }

	private:
		void _physics_process(float delta);
		void _process(float delta);
		void processImpact();
		void processMovement();
		void playAttackSound();

		void deactivate();
		bool collisionDetected();

		const static int64_t SLOW = 150;
		const static int64_t INITIAL_ENTANGLING_BALLS_SPEED = 1200;
		const static int64_t GRAVITY_PULL = 20;
		const static int64_t AIR_RESISTANCE = 10;
		const static int64_t MAX_FALLING_SPEED = 1800;
		const static int64_t MAX_DISTANCE = 350;
		const static int64_t SLOW_DURATION = 5;

		Vector2 velocity_;
		Vector2 initialPosition_;
		Vector2 initialDirection_;
		int64_t shooterNodeName_;

		Vector2 slavePosition_;
		real_t slaveRotation_;

		bool activated_;
		bool objectHit_;
    };
}
