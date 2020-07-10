#pragma once
#include "Role.h"

namespace godot
{
	class Warrior : public Role
	{
	public:
		Warrior() = default;
		Warrior(Player* newOwner);
		~Warrior() = default;

		void setUI();
		void updateSprite();

		void useSecondAbility();
		void useThirdAbility();
		void useFourthAbility();
		void neutralizeFirstEffect();
		void neutralizeSecondEffect();

	private:
		bool entanglingBallsOnCooldown();
		bool chargeOnCooldown();
		bool stoneSkinOnCooldown();

		Ref<PackedScene> entanglingBallsScene_;
		const static int64_t ENTANGLING_BALLS_COOLDOWN = 10;
		const static int64_t CHARGE_COOLDOWN = 10;
		const static int64_t STONE_SKIN_COOLDOWN = 20;
		const static int64_t CHARGE_DURATION = 3;
		const static int64_t STONE_SKIN_DURATION = 5;
	};
}
