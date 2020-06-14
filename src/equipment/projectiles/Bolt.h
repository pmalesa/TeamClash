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
        void _ready();
		void init(String shooterNodeName, Vector2 initialPosition, Vector2 initialDirection);

	private:
		void _physics_process(float delta);
		void _process(float delta);
		void processImpact();
		void processBoltMovement();
		void _on_BoltLifeAfterHitTimer_timeout();
		void _on_BoltMaximumLifeTimer_timeout();

		bool collisionDetected();

		const static int64_t damage_ = 35;
		const static int64_t INITIAL_PROJECTILE_SPEED = 1700;
		const static int64_t GRAVITY_PULL = 20;
		const static int64_t AIR_RESISTANCE = 2;
		const static int64_t MAX_FALLING_SPEED = 1800;
		Vector2 velocity_;
		Vector2 initialDirection_;
		String shooterNodeName_;
		bool objectHit_;

    };
}
