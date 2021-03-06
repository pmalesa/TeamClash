#pragma once

#include "Role.h"
#include <Timer.hpp>

namespace godot
{
	class PlayerUI;

	class Archer : public Role
	{
	public:
		Archer() = default;
		Archer(Player* newOwner);
		~Archer() = default;

		void setupUI();
		void updateSprite();

		void useFirstAbility();
		void useSecondAbility();
		void useThirdAbility();
		void useFourthAbility();
		void useAdditionalAbility();

		void neutralizeFirstEffect();

		bool trapOnCooldown() { return static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown"))->get_time_left() > 0; }
		bool rapidFireOnCooldown() { return static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown"))->get_time_left() > 0; }

	private:
		void switchWeapon();
		void switchAmmoType();
		void placeTrap();
		void rapidFire();
		void updateArmRotation();

		void setProjectileTypeTo(ProjectileType newProjectileType);

		ArcherUI* ui_;
		Ref<PackedScene> trapScene_;
		const static int64_t DEFAULT_BOLT_COOLDOWN = 1;
		const static int64_t DEFAULT_EXPLOSIVE_BOLT_COOLDOWN = 5;
		const static int64_t TRAP_COOLDOWN = 10;
		const static int64_t RAPID_FIRE_COOLDOWN = 20;
		const static int64_t RAPID_FIRE_DURATION = 8;
	};
}