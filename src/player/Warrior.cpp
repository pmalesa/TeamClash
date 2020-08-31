#include "Warrior.h"

#include "ui/WarriorUI.h"

#include "../equipment/weapons/Axe.h"
#include "../equipment/utility/EntanglingBalls.h"

#include <Ref.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <TextureRect.hpp>
#include <Texture.hpp>
#include <AnimatedSprite.hpp>
#include <Sprite.hpp>
#include <AnimationPlayer.hpp>
#include <AudioStreamPlayer.hpp>
#include <CanvasLayer.hpp>

#include <Godot.hpp>
#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

Warrior::Warrior(Player* newOwner) : Role(newOwner)
{
	getOwner()->weapons_.clear();
	getOwner()->weapons_.push_back(static_cast<Axe*>(static_cast<Ref<PackedScene>>(getOwner()->resourceLoader_->load("res://equipment/weapons/Axe.tscn"))->instance()));
	getOwner()->get_node("melee_weapon_node")->add_child(getOwner()->weapons_[0]);
	getOwner()->currentWeapon_ = static_cast<Axe*>(getOwner()->weapons_[0]);
	entanglingBallsScene_ = getOwner()->resourceLoader_->load("res://equipment/utility/EntanglingBalls.tscn");

	static_cast<Timer*>(getOwner()->get_node("SecondAbilityCooldown"))->set_wait_time(ENTANGLING_BALLS_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown"))->set_wait_time(CHARGE_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown"))->set_wait_time(STONE_SKIN_COOLDOWN);
	static_cast<Timer*>(getOwner()->get_node("FirstEffectTimer"))->set_wait_time(CHARGE_DURATION);
	static_cast<Timer*>(getOwner()->get_node("SecondEffectTimer"))->set_wait_time(STONE_SKIN_DURATION);

	Ref<PackedScene> warriorEffectsScene = getOwner()->resourceLoader_->load("res://player/WarriorEffects.tscn");
	getOwner()->get_node("ClassEffects")->add_child(static_cast<Node2D*>(warriorEffectsScene->instance()));
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeOnAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinOnAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinOnAnimatedSprite"))->set_visible(false);
}

void Warrior::setupUI()
{
	Ref<PackedScene> WarriorUIScene = getOwner()->resourceLoader_->load("res://player/ui/WarriorUI.tscn");
	getOwner()->get_node("/root/Game/UI")->add_child(static_cast<WarriorUI*>(WarriorUIScene->instance()));
	static_cast<CanvasLayer*>(getOwner()->get_node("/root/Game/UI/WarriorUI"))->set_offset(Vector2(750, 950));
	ui_ = static_cast<WarriorUI*>(getOwner()->get_node("/root/Game/UI/WarriorUI"));
}

void Warrior::updateSprite()
{
	AnimatedSprite* bodySprite = static_cast<AnimatedSprite*>(getOwner()->get_node("body_sprite"));
	AnimatedSprite* leftHandSprite = static_cast<AnimatedSprite*>(getOwner()->get_node("left_hand_sprite"));
	AnimatedSprite* rightHandSprite = static_cast<AnimatedSprite*>(getOwner()->get_node("right_hand_sprite"));
	AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(getOwner()->get_node("melee_weapon_node/Weapon/melee_weapon_animation"));
	Node2D* meleeWeaponNode = static_cast<Node2D*>(getOwner()->get_node("melee_weapon_node"));
	Node2D* rangedWeaponNode = static_cast<Node2D*>(getOwner()->get_node("ranged_weapon_node"));

	if (getOwner()->currentWeapon_->getWeaponState() == WeaponState::IDLE)
		rightHandSprite->play("idle" + getOwner()->animationNameSuffix_);
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
		weaponAnimation->play("attack");
		rightHandSprite->set_frame(0);
		rightHandSprite->play("melee_attack" + getOwner()->animationNameSuffix_);
	}
}

void Warrior::useSecondAbility()
{
	if (!entanglingBallsOnCooldown())
	{
		EntanglingBalls* entanglingBalls = static_cast<EntanglingBalls*>(entanglingBallsScene_->instance());
		Vector2 initialPosition = getOwner()->get_position() + 40 * getOwner()->aimingDirection_;
		entanglingBalls->init(getOwner()->getNodeName(), initialPosition, getOwner()->aimingDirection_);
		getOwner()->get_node("/root/Game/World")->add_child(entanglingBalls);
		static_cast<Timer*>(getOwner()->get_node("SecondAbilityCooldown"))->start();
	}
}

void Warrior::useThirdAbility()
{
	if (!chargeOnCooldown())
	{
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeAnimatedSprite"))->set_frame(1);
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeAnimatedSprite"))->play("charge");
		static_cast<AudioStreamPlayer*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeSound"))->play();
		getOwner()->currentMovementSpeed_ *= 1.5;
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeOnAnimatedSprite"))->set_frame(1);
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeOnAnimatedSprite"))->play("charge_on");
		static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown"))->start();
		static_cast<Timer*>(getOwner()->get_node("FirstEffectTimer"))->start();
	}
}

void Warrior::useFourthAbility()
{
	if (!stoneSkinOnCooldown())
	{
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinAnimatedSprite"))->set_frame(1);
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinAnimatedSprite"))->play("stoneskin");
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinOnAnimatedSprite"))->set_visible(true);
		static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinOnAnimatedSprite"))->play("on_effect");
		static_cast<AudioStreamPlayer*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinSound"))->play();
		getOwner()->damageFactor_ = 0.25;
		static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown"))->start();
		static_cast<Timer*>(getOwner()->get_node("SecondEffectTimer"))->start();
	}
}

void Warrior::neutralizeFirstEffect()
{
	getOwner()->currentMovementSpeed_ = getOwner()->DEFAULT_MOVEMENT_SPEED;
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeOnAnimatedSprite"))->stop();
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/Charge/ChargeOnAnimatedSprite"))->set_frame(0);
}

void Warrior::neutralizeSecondEffect()
{
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinOnAnimatedSprite"))->stop();
	static_cast<AnimatedSprite*>(getOwner()->get_node("ClassEffects/Warrior/StoneSkin/StoneSkinOnAnimatedSprite"))->set_visible(false);
	getOwner()->damageFactor_ = 1;
}