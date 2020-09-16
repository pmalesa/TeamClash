#pragma once
#include "UI.h"

#include <Godot.hpp>

namespace godot
{
    class WarriorUI : public UI
    {
        GODOT_CLASS(WarriorUI, UI)

    public:
        static void _register_methods();

		WarriorUI() = default;
        ~WarriorUI() = default;

        void _init() override;
		void _ready() override;

	private:
		void _process(float delta) override;
		void updateCooldownBars() override;
	};
}
