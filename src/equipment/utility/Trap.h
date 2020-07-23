#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

namespace godot
{
	enum class TrapState : int64_t { PLACEMENT, SETUP, ARMED, TRIGGERED };

    class Trap : public KinematicBody2D
    {
        GODOT_CLASS(Trap, KinematicBody2D)

    public:
        static void _register_methods();

		Trap() = default;
        ~Trap() = default;

        void _init();
        void _ready();
		void init(String ownerNodeName, Vector2 initialPosition);

	private:
		void _physics_process(float delta);
		void _process(float delta);
		void _on_PlacementTimer_timeout();
		void _on_LifeAfterTriggerTimer_timeout();

		void processMovement();
		void setup();
		void processTrigger();
		bool collisionDetected();

		const static int64_t MAX_FALLING_SPEED = 1500;
		const static int64_t GRAVITY_PULL = 20;
		const static int64_t IMMOBILIZE_TIME = 3;

		String ownerNodeName_;
		Vector2 velocity_;
		Vector2 initialPosition_;
		TrapState currentState_;
    };
}
