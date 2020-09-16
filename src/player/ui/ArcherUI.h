#pragma once
#include "UI.h"

#include <Godot.hpp>

namespace godot
{
    class ArcherUI : public UI
    {
        GODOT_CLASS(ArcherUI, UI)

    public:
        static void _register_methods();

		ArcherUI() = default;
        ~ArcherUI() = default;

        void _init() override;
		void _ready() override;

	private:
		void _process(float delta) override;
		void updateCooldownBars() override;
	};
}
