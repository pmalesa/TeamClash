#pragma once

#include "Role.h"
#include <Timer.hpp>

namespace godot
{
	class PlayerUI;

	class Warrior : public Role
	{
	public:
		Warrior() = default;
		Warrior(Player* newOwner);
		~Warrior() = default;

		void setupUI();
		void updateSprite();

		void useSecondAbility();
		void useThirdAbility();
		void useFourthAbility();
		void neutralizeFirstEffect();
		void neutralizeSecondEffect();

		bool entanglingBallsOnCooldown() { return static_cast<Timer*>(getOwner()->get_node("SecondAbilityCooldown"))->get_time_left() > 0; }
		bool chargeOnCooldown() { return static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown"))->get_time_left() > 0; }
		bool stoneSkinOnCooldown() { return static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown"))->get_time_left() > 0; }

	private:
		WarriorUI* ui_;
		Ref<PackedScene> entanglingBallsScene_;
		const static int64_t ENTANGLING_BALLS_COOLDOWN = 10;
		const static int64_t CHARGE_COOLDOWN = 10;
		const static int64_t STONE_SKIN_COOLDOWN = 20;
		const static int64_t CHARGE_DURATION = 3;
		const static int64_t STONE_SKIN_DURATION = 5;
	};
}
