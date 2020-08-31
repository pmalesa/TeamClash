#pragma once
#include "Player.h"

#include <PackedScene.hpp>
#include <memory>

namespace godot
{
	class Role
	{
	public:
		Role() = default;
		Role(Player* newOwner) { owner_ = newOwner; }
		virtual ~Role() = default;

		virtual void setupUI() {}
		virtual void updateSprite() {}

		virtual void useFirstAbility() {}
		virtual void useSecondAbility() {}
		virtual void useThirdAbility() {}
		virtual void useFourthAbility() {}
		virtual void useAdditionalAbility() {}

		virtual void neutralizeFirstEffect() {}
		virtual void neutralizeSecondEffect() {}

		Player* getOwner() const { return owner_; }

	private:
		Player* owner_;
	};
}
