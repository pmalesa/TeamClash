#pragma once

#include <Godot.hpp>
#include <Camera2D.hpp>

namespace godot
{

    class Camera : public Camera2D
    {
        GODOT_CLASS(Camera, Camera2D)

    public:
        Camera() = default;
        ~Camera() = default;

        static void _register_methods();

        void _init();
        void _ready();
        void _process(float delta);

    };

}
