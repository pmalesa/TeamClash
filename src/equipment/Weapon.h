#pragma once

#include <Godot.hpp>
#include <Area2D.hpp>

namespace godot
{

    class Weapon : public Area2D
    {
        GODOT_CLASS(Weapon, Area2D)

    public:
        static void _register_methods();

        Weapon() = default;
        ~Weapon() = default;

        void _init();
        void _ready();

    };

}
