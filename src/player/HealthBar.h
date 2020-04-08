#pragma once

#include <Godot.hpp>
#include <TextureProgress.hpp>
#include <Color.hpp>

namespace godot
{
    class HealthBar : public TextureProgress
    {
        GODOT_CLASS(HealthBar, TextureProgress)

    public:
        static void _register_methods();

		HealthBar() = default;
        ~HealthBar() = default;

        void _init();
        void _ready();

		void setValue(int64_t newValue);

	private:

    };
}
