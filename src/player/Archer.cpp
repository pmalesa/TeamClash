#include "Archer.h"

#include "ui/ArcherUI.h"

#include "../equipment/weapons/Dagger.h"
#include "../equipment/weapons/Crossbow.h"
#include "../equipment/utility/Trap.h"

#include <Godot.hpp>
#include <Ref.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <TextureRect.hpp>
#include <Texture.hpp>
#include <AnimatedSprite.hpp>
#include <AnimationPlayer.hpp>
#include <AudioStreamPlayer.hpp>
#include <CanvasLayer.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

Archer::Archer(Player* newOwner) : Role(newOwner)
{
	getOwner()->weapons_.clear();
	getOwner()->weapons_.push_back(static_cast<Dagger*>(static_cast<Ref<PackedScene>>(getOwner()->resourceLoader_->load("res://equipment/weapons/Dagger.tscn"))->instance()));
	getOwner()->weapons_.push_back(static_cast<Crossbow*>(static_cast<Ref<PackedScene>>(getOwner()->resourceLoader_->load("res://equipment/weapons/Crossbow.tscn"))->instance()));
	getOwner()->get_node("melee_weapon_node")->add_child(getOwner()->weapons_[0]);
	getOwner()->get_node("ranged_weapon_node")->add_child(getOwner()->weapons_[1]);
	static_cast<Node2D*>(getOwner()->get_node("melee_weapon_node"))->set_visible(false);

	getOwner()->currentWeapon_ = static_cast<Crossbow*>(getOwner()->weapons_[1]);
	setProjectileTypeTo(ProjectileType::BOLT);
	trapScene_ = getOwner()->resourceLoader_->load("res://equipment/utility/Trap.tscn");

	static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("BoltCooldown"))->set_wait_time(DEFAULT_BOLT_COOLDOWN);
	static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("ExplosiveBoltCooldown"))->set_wait_time(DEFAULT_EXPLOSIVE_BOLT_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown"))->set_wait_time(TRAP_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown"))->set_wait_time(RAPID_FIRE_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("FirstEffectTimer"))->set_wait_time(RAPID_FIRE_DURATION);

	Ref<PackedScene> archerEffectsScene = getOwner()->resourceLoader_->load("res://player/ArcherEffects.tscn");
	getOwner()->get_node("ClassEffects")->add_child(static_cast<Node2D*>(archerEffectsScene->instance()));
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Archer/RapidFire/RapidFireAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Archer/RapidFire/RapidFireAnimatedSprite"))->set_visible(false);
}

void Archer::setupUI()
{
	Ref<PackedScene> ArcherUIScene = getOwner()->resourceLoader_->load("res://player/ui/ArcherUI.tscn");
	getOwner()->get_node("/root/Game/UI")->add_child(static_cast<ArcherUI*>(ArcherUIScene->instance()));
	static_cast<CanvasLayer*>(getOwner()->get_node("/root/Game/UI/ArcherUI"))->set_offset(Vector2(750, 950));
	ui_ = static_cast<ArcherUI*>(getOwner()->get_node("/root/Game/UI/ArcherUI"));
}

void Archer::updateSprite()
{
	AnimatedSprite* bodySprite = static_cast<AnimatedSprite*>(getOwner()->get_node("body_sprite"));
	AnimatedSprite* leftHandSprite = static_cast<AnimatedSprite*>(getOwner()->get_node("left_hand_sprite"));
	AnimatedSprite* rightHandSprite = static_cast<AnimatedSprite*>(getOwner()->get_node("right_hand_sprite"));
	AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(getOwner()->get_node("melee_weapon_node/Weapon/melee_weapon_animation"));
	Node2D* meleeWeaponNode = static_cast<Node2D*>(getOwner()->get_node("melee_weapon_node"));
	Node2D* rangedWeaponNode = static_cast<Node2D*>(getOwner()->get_node("ranged_weapon_node"));

	if (getOwner()->currentWeapon_->isRanged())
	{
		rightHandSprite->play("idle_ranged_weapon" + getOwner()->animationNameSuffix_);
		updateArmRotation();
	}
	else
	{
		if (getOwner()->currentWeapon_->getWeaponState() == WeaponState::IDLE)
			rightHandSprite->play("idle" + getOwner()->animationNameSuffix_);
	}

    if (getOwner()->moveDirection_ == MoveDirection::RIGHT && !getOwner()->immobilized_)
    {
        bodySprite->play("walk" + getOwner()->animationNameSuffix_);
        bodySprite->set_flip_h(false);
		rightHandSprite->set_flip_h(false);
        rightHandSprite->set_z_index(2);
		leftHandSprite->play("walk" + getOwner()->animationNameSuffix_);
        leftHandSprite->set_flip_h(false);
        leftHandSprite->set_z_index(1);
		meleeWeaponNode->set_z_index(-1);
		rangedWeaponNode->set_z_index(-1);
		meleeWeaponNode->set_scale(Vector2(1, meleeWeaponNode->get_scale().y));
		rangedWeaponNode->set_scale(Vector2(1, rangedWeaponNode->get_scale().y));
		getOwner()->facingDirection_ = Vector2(1, 0);
    }
    else if (getOwner()->moveDirection_ == MoveDirection::LEFT && !getOwner()->immobilized_)
    {
        bodySprite->play("walk" + getOwner()->animationNameSuffix_);
        bodySprite->set_flip_h(true);
		rightHandSprite->set_flip_h(true);
        rightHandSprite->set_z_index(-3);
		leftHandSprite->play("walk" + getOwner()->animationNameSuffix_);
        leftHandSprite->set_flip_h(true);
        leftHandSprite->set_z_index(3);
		meleeWeaponNode->set_z_index(-2);
		rangedWeaponNode->set_z_index(-2);
		meleeWeaponNode->set_scale(Vector2(-1, meleeWeaponNode->get_scale().y));
		rangedWeaponNode->set_scale(Vector2(-1, rangedWeaponNode->get_scale().y));
		getOwner()->facingDirection_ = Vector2(-1, 0);
    }
    else
    {
        bodySprite->play("idle" + getOwner()->animationNameSuffix_);
		leftHandSprite->play("idle" + getOwner()->animationNameSuffix_);
    }
    if (!weaponAnimation->is_playing() && getOwner()->currentWeapon_->getWeaponState() == WeaponState::ATTACKING)
    {
		if (!getOwner()->currentWeapon_->isRanged())
			weaponAnimation->play("attack");
		rightHandSprite->set_frame(0);
		if (!getOwner()->currentWeapon_->isRanged())
			rightHandSprite->play("melee_attack" + getOwner()->animationNameSuffix_);
    }
}

void Archer::useFirstAbility()
{
	switchWeapon();
}

void Archer::useSecondAbility()
{
	switchWeapon();
}

void Archer::useThirdAbility()
{
	placeTrap();
}

void Archer::useFourthAbility()
{
	rapidFire();
}

void Archer::useAdditionalAbility()
{
	switchAmmoType();
}

void Archer::neutralizeFirstEffect()
{
	static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("BoltCooldown"))->set_wait_time(DEFAULT_BOLT_COOLDOWN);
	static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("ExplosiveBoltCooldown"))->set_wait_time(DEFAULT_EXPLOSIVE_BOLT_COOLDOWN);
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Archer/RapidFire/RapidFireAnimatedSprite"))->set_visible(false);
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Archer/RapidFire/RapidFireAnimatedSprite"))->stop();
}

void Archer::switchWeapon()
{
	AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(getOwner()->get_node("melee_weapon_node/Weapon/melee_weapon_animation"));
	if (getOwner()->currentWeapon_->getWeaponType() == WeaponType::CROSSBOW)
	{
		getOwner()->currentWeapon_ = getOwner()->weapons_[0];
		static_cast<Node2D*>(getOwner()->get_node("melee_weapon_node"))->set_visible(true);
		static_cast<Node2D*>(getOwner()->get_node("ranged_weapon_node"))->set_visible(false);
		static_cast<AnimatedSprite*>(getOwner()->get_node("right_hand_sprite"))->set_rotation(0);
		if (getOwner()->is_network_master())
		{
			static_cast<TextureRect*>(ui_->get_node("Slot1/Highlight"))->set_visible(true);
			static_cast<TextureRect*>(ui_->get_node("Slot2/Highlight"))->set_visible(false);
			getOwner()->currentWeapon_->playDrawSound();
		}
	}
	else if (getOwner()->currentWeapon_->getWeaponType() == WeaponType::DAGGER && !weaponAnimation->is_playing())
	{
		getOwner()->currentWeapon_ = getOwner()->weapons_[1];
		static_cast<Node2D*>(getOwner()->get_node("melee_weapon_node"))->set_visible(false);
		static_cast<Node2D*>(getOwner()->get_node("ranged_weapon_node"))->set_visible(true);
		if (getOwner()->is_network_master())
		{
			static_cast<TextureRect*>(ui_->get_node("Slot1/Highlight"))->set_visible(false);
			static_cast<TextureRect*>(ui_->get_node("Slot2/Highlight"))->set_visible(true);
			getOwner()->currentWeapon_->playDrawSound();
		}
	}
}

void Archer::switchAmmoType()
{
	if (static_cast<Crossbow*>(getOwner()->weapons_[1])->getAmmoType() == ProjectileType::BOLT)
	{
		setProjectileTypeTo(ProjectileType::EXPLOSIVE_BOLT);
		if (getOwner()->is_network_master())
		{
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(false);
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot2/Highlight"))->set_visible(true);
		}
	}
	else if (static_cast<Crossbow*>(getOwner()->weapons_[1])->getAmmoType() == ProjectileType::EXPLOSIVE_BOLT)
	{
		setProjectileTypeTo(ProjectileType::BOLT);
		if (getOwner()->is_network_master())
		{
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(true);
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot2/Highlight"))->set_visible(false);
		}
	}
}

void Archer::placeTrap()
{
	if (!trapOnCooldown())
	{
		if (getOwner()->get_tree()->is_network_server())
		{
			Trap* trap = static_cast<Trap*>(getOwner()->get_node("/root/Game")->call("takeTrapFromStack"));
			if (!trap)
				return;
			Vector2 initialPosition = getOwner()->get_position() +  60 * getOwner()->facingDirection_;
			getOwner()->get_node("/root/Game")->rpc("activateTrap", trap->get_name(), getOwner()->get_name(), initialPosition - Vector2(0, 40));
		}
		static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown"))->start();
	}
}

void Archer::rapidFire()
{
	if (!rapidFireOnCooldown())
	{
		static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown"))->start();
		static_cast<Timer*>(getOwner()->get_node("FirstEffectTimer"))->start();
		static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("BoltCooldown"))->stop();
		static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("BoltCooldown"))->set_wait_time(0.2);
		static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("ExplosiveBoltCooldown"))->stop();
		static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("ExplosiveBoltCooldown"))->set_wait_time(1);
		static_cast<AudioStreamPlayer*>(getOwner()->get_node("ClassEffects/Archer/RapidFire/RapidFireSound"))->play();
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Archer/RapidFire/RapidFireAnimatedSprite"))->set_visible(true);
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Archer/RapidFire/RapidFireAnimatedSprite"))->play();
	}
}

void Archer::updateArmRotation()
{
	real_t angle = getOwner()->facingDirection_.angle_to(getOwner()->aimingDirection_);
	static_cast<AnimatedSprite*>(getOwner()->get_node("right_hand_sprite"))->set_rotation(angle);
	static_cast<Crossbow*>(getOwner()->get_node("ranged_weapon_node"))->set_rotation(angle);
}

void Archer::setProjectileTypeTo(ProjectileType newProjectileType)
{
	static_cast<Crossbow*>(getOwner()->weapons_[1])->setAmmoType(newProjectileType);
}