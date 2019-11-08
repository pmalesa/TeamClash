#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

namespace godot
{

    enum class MoveDirection : int64_t
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NONE
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
        void _move(int64_t direction);
        void damage(int64_t damage);
        void _die();
        void _on_RespawnTimer_timeout();
        void init(String nickname, Vector2 startPosition, bool isSlave);

    private:
        const float MOVE_SPEED = 10.0f;
        const int64_t MAX_HP = 100;

        Vector2 slavePosition;
        int64_t slaveMovement;
        int64_t healthPoints = MAX_HP;
        int64_t nodeName;

    };

}
