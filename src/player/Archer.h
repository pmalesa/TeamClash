#pragma once

#include "Role.h"

namespace godot
{
	class Archer : public Role
	{
	public:
		Archer() = default;
		Archer(Player* newOwner);
		~Archer() = default;

		void setUI();
		void updateSprite();

		void useFirstAbility();
		void useSecondAbility();
		void useThirdAbility();
		void useFourthAbility();
		void useAdditionalAbility();

	private:
		void switchWeapon();
		void switchAmmoType();
		void updateArmRotation();

		void setProjectileTypeTo(ProjectileType newProjectileType);

		const static int64_t DEFAULT_BOLT_COOLDOWN = 1;
		const static int64_t EXPLOSIVE_BOLT_COOLDOWN = 5;
		const static int64_t TRAP_COOLDOWN = 10;
		const static int64_t RAPID_FIRE_COOLDOWN = 20;
		const static int64_t RAPID_FIRE_DURATION = 8;

		int64_t currentBoltCooldown_;
	};
}