#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>
#include "../../player/Player.h"

#include <iostream>

namespace godot
{
    class ExplosiveBolt : public KinematicBody2D
    {
        GODOT_CLASS(ExplosiveBolt, KinematicBody2D)

    public:
        static void _register_methods();

		ExplosiveBolt() = default;
        ~ExplosiveBolt() = default;

        void _init();
        void _ready();
		void init(int64_t shooterNodeName, Vector2 initialPosition, Vector2 initialDirection);

	private:
		void _physics_process(float delta);
		void _process(float delta);
		void processImpact();
		void processBoltMovement();
		void _on_ExplosiveBoltMaximumLifeTimer_timeout();
		void _on_ExplosiveBoltAfterExplosionLifeTime_timeout();

		bool collisionDetected();

		const static int64_t damage_ = 75;
		const static int64_t INITIAL_PROJECTILE_SPEED = 1200;
		const static int64_t GRAVITY_PULL = 20;
		const static int64_t AIR_RESISTANCE = 3;
		const static int64_t MAX_FALLING_SPEED = 1600;
		const static int64_t EXPLOSION_THROWBACK = 1000;
		Vector2 velocity_;
		Vector2 initialDirection_;
		int64_t shooterNodeName_;
		bool objectHit_;

    };
}



/* 
	Note:

	Strange thing happens when the above's private variables are declared only as const and are assigned with some values.
	During the program execution, those constant variables have different values than they were assigned with and those 
	values CHANGE (!), even though the variables are constant.

	Disable possibility to hit yourself with your own bolt.

*/