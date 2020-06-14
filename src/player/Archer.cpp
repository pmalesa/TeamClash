#include "Archer.h"

#include "../equipment/weapons/Dagger.h"
#include "../equipment/weapons/Crossbow.h"

#include <Godot.hpp>
#include <Ref.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <TextureRect.hpp>
#include <Texture.hpp>
#include <AnimatedSprite.hpp>
#include <AnimationPlayer.hpp>

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
	currentBoltCooldown_ = DEFAULT_BOLT_COOLDOWN;
	static_cast<Timer*>(getOwner()->get_node("FirstAbilityCooldown"))->set_wait_time(DEFAULT_BOLT_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("SecondAbilityCooldown"))->set_wait_time(EXPLOSIVE_BOLT_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown"))->set_wait_time(TRAP_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown"))->set_wait_time(RAPID_FIRE_COOLDOWN);
}

void Archer::setUI()
{
	static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot1/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/dagger_icon.png"));
	static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/crossbow_icon.png"));
	static_cast<CanvasItem*>(getOwner()->ui_->get_node("Slot2/Subslot1"))->set_visible(true);
	static_cast<CanvasItem*>(getOwner()->ui_->get_node("Slot2/Subslot2"))->set_visible(true);
	static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Highlight"))->set_visible(true);
	static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(true);
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

    if (getOwner()->moveDirection_ == MoveDirection::RIGHT)
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
    else if (getOwner()->moveDirection_ == MoveDirection::LEFT)
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

}

void Archer::useFourthAbility()
{

}

void Archer::useAdditionalAbility()
{
	switchAmmoType();
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
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot1/Highlight"))->set_visible(true);
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Highlight"))->set_visible(false);
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
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot1/Highlight"))->set_visible(false);
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Highlight"))->set_visible(true);
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
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(false);
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Subslot2/Highlight"))->set_visible(true);
		}
	}
	else if (static_cast<Crossbow*>(getOwner()->weapons_[1])->getAmmoType() == ProjectileType::EXPLOSIVE_BOLT)
	{
		setProjectileTypeTo(ProjectileType::BOLT);
		if (getOwner()->is_network_master())
		{
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(true);
			static_cast<TextureRect*>(getOwner()->ui_->get_node("Slot2/Subslot2/Highlight"))->set_visible(false);
		}
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