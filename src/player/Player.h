#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

namespace godot
{

    enum class MoveDirection : int64_t
    {
        LEFT,
        RIGHT,
        NONE
    };

	enum class MovementState : int64_t
	{
		NONE,
		JUMPED,
		FALLING
	};

    class Player : public KinematicBody2D
    {
        GODOT_CLASS(Player, KinematicBody2D)

    public:
        static void _register_methods();

        Player();
        ~Player();

        void _init();
        void _ready();

        void _physics_process(float delta);
		void _process(float delta);
        void _move(int64_t direction);
		void _on_HealthBar_value_changed(float value);
        void damage(int64_t damage);
		void updateKeyboardInput();
        void _die();
        void _on_RespawnTimer_timeout();
        void init(String nickname, Vector2 startPosition, bool isSlave);

		Vector2 getVelocity() const { return velocity_; };
		String getNickname() const { return nickname_; }
		void setVelocity(Vector2 velocity) { velocity_ = velocity; }
		void addVelocity(Vector2 velocity) { velocity_ += velocity; }

    private:
		String nickname_;
        const float MOVE_SPEED = 300.0f;
        const int64_t MAX_HP = 100;
		const float JUMP_POWER = 600.0f;
		const float GRAVITY_PULL = 20.0f;
		Vector2 velocity_;
		MoveDirection moveDirection_;
		MovementState movementState_;

        Vector2 slavePosition;
        int64_t slaveMovement;
        int64_t healthPoints = MAX_HP;
        int64_t nodeName;
    };

}
