#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

namespace godot
{
	enum class TrapState : int64_t {DEACTIVATED, PLACEMENT, SETUP, ARMED, TRIGGERED };

    class Trap : public KinematicBody2D
    {
        GODOT_CLASS(Trap, KinematicBody2D)

    public:
        static void _register_methods();

		Trap() = default;
        ~Trap() = default;

        void _init();
        void _ready();
		void activate(String ownerNodeName, Vector2 initialPosition);
		void deactivate();

	private:
		void _physics_process(float delta);
		void _process(float delta);
		void _on_PlacementTimer_timeout();
		void _on_LifeAfterTriggerTimer_timeout();

		void processMovement();
		void setup();
		void processTrigger();
		bool playerCollisionDetected();
		void playTriggerAnimation();

		const static int64_t MAX_FALLING_SPEED = 1500;
		const static int64_t GRAVITY_PULL = 20;
		const static int64_t IMMOBILIZE_TIME = 3;

		Vector2 velocity_;
		Vector2 initialPosition_;
		String ownerNodeName_;
		TrapState currentState_;

		Vector2 slavePosition_;

		bool activated_;
    };
}
