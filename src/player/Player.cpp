#include "Player.h"

#include "../equipment/weapons/Dagger.h"
#include "../equipment/weapons/Axe.h"
#include "../equipment/weapons/Crossbow.h"
#include "../equipment/projectiles/Bolt.h"
#include "../equipment/projectiles/ExplosiveBolt.h"
#include "../equipment/utility/EntanglingBalls.h"

#include <SceneTree.hpp>
#include <KinematicCollision2D.hpp>
#include <CollisionShape2D.hpp>
#include <Label.hpp>
#include <Texture.hpp>
#include <TextureRect.hpp>
#include <Array.hpp>
#include <Input.hpp>
#include <SceneTree.hpp>
#include <AnimatedSprite.hpp>
#include <SpriteFrames.hpp>
#include <Sprite.hpp>
#include <AnimationPlayer.hpp>
#include <AudioStreamPlayer.hpp>
#include <array>
#include <Viewport.hpp>
#include <PackedScene.hpp>
#include <ResourceLoader.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void Player::_register_methods()
{
    register_method("_physics_process", &Player::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Player::_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Player::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Player::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_SlowTimer_timeout", &Player::_on_SlowTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_RespawnTimer_timeout", &Player::_on_RespawnTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_on_BoltCooldown_timeout", &Player::_on_BoltCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ExplosiveBoltCooldown_timeout", &Player::_on_ExplosiveBoltCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_EntanglingBallsCooldown_timeout", &Player::_on_EntanglingBallsCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setTeam", &Player::setTeam, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("setRole", &Player::setRole, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("setUI", &Player::setUI, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setSpawnPoint", &Player::setSpawnPoint, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
    register_method("_move", &Player::_move, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("inflictDamage", &Player::inflictDamage, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("inflictSlow", &Player::inflictSlow, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("playBodyHitSound", &Player::playBodyHitSound, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("throwback", &Player::throwback, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("updateInput", &Player::updateInput, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("updateSprite", &Player::updateSprite, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_die", &Player::_die, GODOT_METHOD_RPC_MODE_REMOTESYNC);
    register_method("init", &Player::init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("updateHealthPoints", &Player::updateHealthPoints, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateMovementSpeed", &Player::updateMovementSpeed, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setSlowTime", &Player::setSlowTime, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateHealthBar", &Player::updateHealthBar, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateArmRotation", &Player::updateArmRotation, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("switchWeapon", &Player::switchWeapon, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("setProjectileTypeTo", &Player::setProjectileTypeTo, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("shootBolt", &Player::shootBolt, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("throwEntanglingBalls", &Player::throwEntanglingBalls, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("showEntanglementEffect", &Player::showEntanglementEffect, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("hideEntanglementEffect", &Player::hideEntanglementEffect, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	
	register_property<Player, int64_t>("healthPoints_", &Player::healthPoints_, 0, GODOT_METHOD_RPC_MODE_DISABLED);
    register_property<Player, Vector2>("slavePosition", &Player::slavePosition, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Player, int64_t>("slaveMovement", &Player::slaveMovement, static_cast<int64_t>(MoveDirection::NONE), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Player, int64_t>("slaveWeaponState", &Player::slaveWeaponState, static_cast<int64_t>(WeaponState::IDLE), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Player, int64_t>("nodeName", &Player::nodeName_, 0, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<Player, Vector2>("facingDirection_", &Player::facingDirection_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Player, Vector2>("aimingDirection_", &Player::aimingDirection_, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Player, Vector2>("throwbackVelocity_", &Player::throwbackVelocity_, Vector2(), GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_property<Player, bool>("applyThrowback_", &Player::applyThrowback_, false, GODOT_METHOD_RPC_MODE_REMOTESYNC);
}

void Player::_init()
{
	resourceLoader_ = ResourceLoader::get_singleton();
	weaponScene_ = resourceLoader_->load("res://equipment/weapons/Weapon.tscn");
	boltScene_ = resourceLoader_->load("res://equipment/projectiles/Bolt.tscn");
	explosiveBoltScene_ = resourceLoader_->load("res://equipment/projectiles/ExplosiveBolt.tscn");
	entanglingBallsScene_ = resourceLoader_->load("res://equipment/utility/EntanglingBalls.tscn");
	healthPoints_ = MAX_HP;
	velocity_ = Vector2(0, 0);
	facingDirection_ = Vector2(1, 0);
	aimingDirection_ = Vector2(0, 0);
	throwbackVelocity_ = Vector2(0, 0);
	applyThrowback_ = false;
	entanglingBallsThrown_ = false;
	moveDirection_ = MoveDirection::NONE;
	movementState_ = MovementState::NONE;
    slavePosition = Vector2();
    slaveMovement = static_cast<int64_t>(MoveDirection::NONE);
    nodeName_ = 0;
	animationNameSuffix_ = String();
	initialized_ = false;

	Godot::print("[PLAYER] Player variables initialized.");
}

void Player::_ready()
{
	nicknameLabel_ = static_cast<Label*>(get_node("NicknameBar/Nickname"));
	healthBar_ = static_cast<HealthBar*>(get_node("HealthBar/HealthBar"));
	ui_ = static_cast<Control*>(get_node("/root/Game/UI/PlayerUI"));
	currentMovementSpeed_ = DEFAULT_MOVEMENT_SPEED;
	nicknameLabel_->set_text(get_node("/root/Network")->call("getConnectedPlayerNickname", nodeName_));
	updateHealthBar();
	Godot::print("[PLAYER] Player ready.");
}

void Player::_physics_process(float delta)
{
	if (is_network_master())
	{
		rset_unreliable("slavePosition", get_position());
		rset("slaveMovement", static_cast<int64_t>(moveDirection_));
        rset("slaveWeaponState", static_cast<int64_t>(weaponState_));
		_move(static_cast<int64_t>(moveDirection_));
	}
	else
	{
		_move(slaveMovement);
		set_position(slavePosition);
	}
}

void Player::_process(float delta)
{
    if (is_network_master())
    {
        updateInput();
		updateAimingDirection();
		processRangedAttack();
		processMeleeAttack();
		processThrow();
		rset("aimingDirection_", aimingDirection_);
		if (currentWeapon_->isRanged())
			rpc("updateArmRotation", aimingDirection_);
	}
    else
    {
        moveDirection_ = MoveDirection(slaveMovement);
        weaponState_ = WeaponState(slaveWeaponState);
    }
	updateSprite();
}

void Player::_move(int64_t direction)
{
    MoveDirection moveDirection = static_cast<MoveDirection>(direction);
	if (applyThrowback_)
	{
		movementState_ = MovementState::THROWBACK;
		applyThrowback_ = false;
	}
	if (is_on_ceiling())
	{
		velocity_.y = 0.0f;
		set_position(Vector2(get_position().x, get_position().y + 1));
	}

	if (movementState_ == MovementState::NONE)
	{
		if (is_on_floor())
			velocity_.y = 10.0f;
		else
			movementState_ = MovementState::FALLING;
	}
	if (movementState_ == MovementState::JUMPED)
	{
		velocity_.y = -JUMP_POWER;
		movementState_ = MovementState::FALLING;
	}
	if (movementState_ == MovementState::THROWBACK)
	{
		throwbackVelocity_.y = -200.0f;
		addVelocity(throwbackVelocity_);
		movementState_ = MovementState::FALLING;
	}
	if (movementState_ == MovementState::FALLING)
	{
		setVelocity(Vector2(getVelocity().x, getVelocity().y + GRAVITY_PULL));

		if (is_on_floor())
			movementState_ = MovementState::NONE;
	}

	switch (moveDirection)
	{
	case MoveDirection::NONE:
		if (throwbackVelocity_.x == 0)
			velocity_.x = 0.0f;
		else
			velocity_.x = throwbackVelocity_.x;
		break;

	case MoveDirection::LEFT:
		if (throwbackVelocity_.x == 0)
			velocity_.x = -currentMovementSpeed_;
		else
			velocity_.x = -currentMovementSpeed_ + throwbackVelocity_.x;
		break;

	case MoveDirection::RIGHT:
		if (throwbackVelocity_.x == 0)
			velocity_.x = currentMovementSpeed_;
		else
			velocity_.x = currentMovementSpeed_ + throwbackVelocity_.x;
		break;
	}

	if (throwbackVelocity_.x > 0)
	{
		throwbackVelocity_.x -= HORIZONTAL_THROWBACK_DECAY;
		if (throwbackVelocity_.x < 0)
			throwbackVelocity_.x = 0;
	}
	if (throwbackVelocity_.x < 0)
	{
		throwbackVelocity_.x += HORIZONTAL_THROWBACK_DECAY;
		if (throwbackVelocity_.x > 0)
			throwbackVelocity_.x = 0;
	}

	throwbackVelocity_.y = 0;
	move_and_slide(velocity_, Vector2(0, -1));
}

void Player::inflictDamage(int64_t value)
{
	healthPoints_ -= value;
	if (healthPoints_ < 0)
		healthPoints_ = 0;

	rpc("playBodyHitSound");
	rpc("updateHealthPoints", healthPoints_);
	rpc("updateHealthBar");
	if (healthPoints_ == 0)
		rpc("_die");
}

void Player::inflictSlow(int64_t slowAmount, int64_t slowTime)
{
	rpc("playBodyHitSound");
	rpc("updateMovementSpeed", slowAmount);
	rpc("setSlowTime", slowTime);
	rpc("showEntanglementEffect");
}

void Player::playBodyHitSound()
{
	static_cast<AudioStreamPlayer*>(get_node("BodyHitSound"))->play();
}

void Player::throwback(Vector2 direction, int64_t throwbackPower)
{
	Vector2 throwbackVelocity = Vector2(0, 0);
	if (direction.x > 0)
		throwbackVelocity.x = throwbackPower;
	else
		throwbackVelocity.x = -throwbackPower;
	if (direction.y > 0)
		throwbackVelocity.y = throwbackPower;
	else
		throwbackVelocity.y = -throwbackPower;
	rset("throwbackVelocity_", throwbackVelocity);
	rset("applyThrowback_", true);
}

void Player::processMeleeAttack()
{
	if (currentWeapon_->isRanged())
		return;

	if (weaponState_ == WeaponState::ATTACKING)
	{
		currentWeapon_->playAttackSound();
		currentWeapon_->set_physics_process(true);
		Array overlapingBodies = currentWeapon_->get_overlapping_bodies();
		if (overlapingBodies.empty())
			return;
		
		for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
		{
			Node* overlappedNode = static_cast<Node*>(overlapingBodies[i]);
			if (!overlappedNode->is_in_group("Player"))
				return;
			else
			{
				Player* attackedPlayer = static_cast<Player*>(overlapingBodies[i]);
				if (!(attackedPlayer == this) && !(alreadyAttackedPlayers_.has(attackedPlayer)))
				{
					attackedPlayer->inflictDamage(currentWeapon_->getDamage());
					if (attackedPlayer->healthPoints_ > 0)
						attackedPlayer->throwback(Vector2(attackedPlayer->get_position().x - get_position().x, attackedPlayer->get_position().y - get_position().y));
					if (!alreadyAttackedPlayers_.has(attackedPlayer))
						alreadyAttackedPlayers_.push_front(attackedPlayer);
				}
			}
		}
		currentWeapon_->set_physics_process(false);
	}
	else
	{
		alreadyAttackedPlayers_.clear();
		return;
	}
}

void Player::processRangedAttack()
{
	if (weaponState_ == WeaponState::SHOOTING)
		rpc("shootBolt");
}

void Player::processThrow()
{
	if (entanglingBallsThrown_)
	{
		rpc("throwEntanglingBalls");
		entanglingBallsThrown_ = false;
	}
}

void Player::shootBolt()
{
	if (!currentWeapon_->isRanged())
		return;

	if (currentAmmoType_ == ProjectileType::BOLT)
	{
		if (!boltOnCooldown())
		{
			Bolt* bolt = static_cast<Bolt*>(boltScene_->instance());
			Vector2 boltInitialPosition = Vector2(get_position().x + aimingDirection_.x * 100, get_position().y + aimingDirection_.y * 100);
			bolt->init(getNodeName(), boltInitialPosition, aimingDirection_);
			get_node("/root/Game/World")->add_child(bolt);
			static_cast<Timer*>(get_node("BoltCooldown"))->start();
			Godot::print("BOLT SHOT!");
		}
	}
	else if (currentAmmoType_ == ProjectileType::EXPLOSIVE_BOLT)
	{
		if (!explosiveBoltOnCooldown())
		{
			ExplosiveBolt* explosiveBolt = static_cast<ExplosiveBolt*>(explosiveBoltScene_->instance());
			Vector2 boltInitialPosition = Vector2(get_position().x + aimingDirection_.x * 40, get_position().y + aimingDirection_.y * 40);
			explosiveBolt->init(getNodeName(), boltInitialPosition, aimingDirection_);
			get_node("/root/Game/World")->add_child(explosiveBolt);
			static_cast<Timer*>(get_node("ExplosiveBoltCooldown"))->start();
			Godot::print("EXPLOSIVE BOLT SHOT!");
		}
	}
}

void Player::throwEntanglingBalls()
{
	if (!entanglingBallsOnCooldown())
	{
		EntanglingBalls* entanglingBalls = static_cast<EntanglingBalls*>(entanglingBallsScene_->instance());
		Vector2 initialPosition = Vector2(get_position().x + aimingDirection_.x * 40, get_position().y + aimingDirection_.y * 40);
		entanglingBalls->init(getNodeName(), initialPosition, aimingDirection_);
		get_node("/root/Game/World")->add_child(entanglingBalls);
		static_cast<Timer*>(get_node("EntanglingBallsCooldown"))->start();
		Godot::print("ENTANGLING BALLS THROWN!");
	}
}

void Player::updateInput()
{
	moveDirection_ = MoveDirection::NONE;
	Input* input = Input::get_singleton();
	AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(get_node("melee_weapon_node/Weapon/melee_weapon_animation"));
	if (input->is_action_pressed("left"))
	{
		moveDirection_ = MoveDirection::LEFT;
	}
	else if (input->is_action_pressed("right"))
	{
		moveDirection_ = MoveDirection::RIGHT;
	}

	if (input->is_action_just_pressed("space"))
	{
		if (is_on_floor()) movementState_ = MovementState::JUMPED;
	}

	if (input->is_action_just_pressed("1"))
	{
		if (Role::ARCHER == static_cast<Role>(role_) && currentWeapon_->getWeaponType() != static_cast<int64_t>(WeaponType::DAGGER))
		{
			rpc("switchWeapon");
			static_cast<TextureRect*>(ui_->get_node("Slot1/Highlight"))->set_visible(true);
			static_cast<TextureRect*>(ui_->get_node("Slot2/Highlight"))->set_visible(false);
		}
	}
	if (input->is_action_just_pressed("2"))
	{
		if (!weaponAnimation->is_playing())
		{
			if (Role::ARCHER == static_cast<Role>(role_) && currentWeapon_->getWeaponType() != static_cast<int64_t>(WeaponType::CROSSBOW))
			{
				rpc("switchWeapon");
				static_cast<TextureRect*>(ui_->get_node("Slot1/Highlight"))->set_visible(false);
				static_cast<TextureRect*>(ui_->get_node("Slot2/Highlight"))->set_visible(true);
			}
			else if (Role::WARRIOR == static_cast<Role>(role_))
				entanglingBallsThrown_ = true;
		}
	}
	if (Role::ARCHER == static_cast<Role>(role_) && input->is_action_just_pressed("f"))
	{
		if (currentAmmoType_ == ProjectileType::BOLT)
		{
			rpc("setProjectileTypeTo", static_cast<int64_t>(ProjectileType::EXPLOSIVE_BOLT));
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(false);
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot2/Highlight"))->set_visible(true);
		}
		else if (currentAmmoType_ == ProjectileType::EXPLOSIVE_BOLT)
		{
			rpc("setProjectileTypeTo", static_cast<int64_t>(ProjectileType::BOLT));
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(true);
			static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot2/Highlight"))->set_visible(false);
		}
	}

	if (currentWeapon_->isRanged())
	{
		if (input->is_action_just_pressed("basic_attack"))
		{
			weaponState_ = WeaponState::SHOOTING;
		}
		else
		{
			weaponState_ = WeaponState::IDLE;
		}
	}
	else
	{
		if (input->is_action_just_pressed("basic_attack"))
		{
			if (!weaponAnimation->is_playing()) weaponState_ = WeaponState::ATTACKING;
		}
		else
		{
			if (!weaponAnimation->is_playing()) weaponState_ = WeaponState::IDLE;
		}
	}
}

void Player::updateSprite()
{
	if (!initialized_)
		return;
	AnimatedSprite* bodySprite = static_cast<AnimatedSprite*>(get_node("body_sprite"));
	AnimatedSprite* leftHandSprite = static_cast<AnimatedSprite*>(get_node("left_hand_sprite"));
	AnimatedSprite* rightHandSprite = static_cast<AnimatedSprite*>(get_node("right_hand_sprite"));
	AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(get_node("melee_weapon_node/Weapon/melee_weapon_animation"));
	Node2D* meleeWeaponNode = static_cast<Node2D*>(get_node("melee_weapon_node"));
	Node2D* rangedWeaponNode = static_cast<Node2D*>(get_node("ranged_weapon_node"));

	if (currentWeapon_->isRanged())
	{
		rightHandSprite->play("idle_ranged_weapon" + animationNameSuffix_);
	}
	else
	{
		if (weaponState_ == WeaponState::IDLE)
			rightHandSprite->play("idle" + animationNameSuffix_);
	}

    if (moveDirection_ == MoveDirection::RIGHT)
    {
        bodySprite->play("walk" + animationNameSuffix_);
        bodySprite->set_flip_h(false);
		rightHandSprite->set_flip_h(false);
        rightHandSprite->set_z_index(2);
		leftHandSprite->play("walk" + animationNameSuffix_);
        leftHandSprite->set_flip_h(false);
        leftHandSprite->set_z_index(1);
		meleeWeaponNode->set_z_index(-1);
		rangedWeaponNode->set_z_index(-1);
		meleeWeaponNode->set_scale(Vector2(1, meleeWeaponNode->get_scale().y));
		rangedWeaponNode->set_scale(Vector2(1, rangedWeaponNode->get_scale().y));
		facingDirection_ = Vector2(1, 0);
    }
    else if (moveDirection_ == MoveDirection::LEFT)
    {
        bodySprite->play("walk" + animationNameSuffix_);
        bodySprite->set_flip_h(true);
		rightHandSprite->set_flip_h(true);
        rightHandSprite->set_z_index(-3);
		leftHandSprite->play("walk" + animationNameSuffix_);
        leftHandSprite->set_flip_h(true);
        leftHandSprite->set_z_index(3);
		meleeWeaponNode->set_z_index(-2);
		rangedWeaponNode->set_z_index(-2);
		meleeWeaponNode->set_scale(Vector2(-1, meleeWeaponNode->get_scale().y));
		rangedWeaponNode->set_scale(Vector2(-1, rangedWeaponNode->get_scale().y));
		facingDirection_ = Vector2(-1, 0);
    }
    else
    {
        bodySprite->play("idle" + animationNameSuffix_);
		leftHandSprite->play("idle" + animationNameSuffix_);
    }

    if (!weaponAnimation->is_playing() && weaponState_ == WeaponState::ATTACKING)
    {
		if (!currentWeapon_->isRanged())
			weaponAnimation->play("attack");
		rightHandSprite->set_frame(0);
		if (!currentWeapon_->isRanged())
			rightHandSprite->play("melee_attack" + animationNameSuffix_);
    }
}

void Player::_die()
{
	if (is_network_master())
		get_node("/root/Game")->call("showRespawnWindow");
		
	static_cast<AudioStreamPlayer*>(get_node("DeathSound"))->play();
    static_cast<Timer*>(get_node("RespawnTimer"))->start();
    set_physics_process(false);

    for (unsigned int i = 0; i < get_child_count(); i++)
    {
        if (get_child(i)->has_method("hide"))
        {
            get_child(i)->call("hide");
        }
    }
    static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(true);
}

void Player::_on_SlowTimer_timeout()
{
	hideEntanglementEffect();
	static_cast<Timer*>(get_node("SlowTimer"))->stop();
	rpc("updateMovementSpeed", DEFAULT_MOVEMENT_SPEED);
}

void Player::_on_RespawnTimer_timeout()
{
	if(is_network_master())
		get_node("/root/Game")->call("hideRespawnWindow");
	
	static_cast<Timer*>(get_node("RespawnTimer"))->stop();
	set_position(spawnPoint_);
    set_physics_process(true);

    for (unsigned int i = 0; i < get_child_count(); i++)
    {
        if (get_child(i)->has_method("show"))
        {
            get_child(i)->call("show");
        }
    }
	if (currentWeapon_->isRanged())
		static_cast<Node2D*>(get_node("melee_weapon_node"))->set_visible(false);
	else
		static_cast<Node2D*>(get_node("ranged_weapon_node"))->set_visible(false);
    static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(false);
	throwbackVelocity_ = Vector2(0, 0);
	applyThrowback_ = false;
    healthPoints_ = MAX_HP;
	updateHealthBar();
}

void Player::_on_BoltCooldown_timeout()
{
	static_cast<Timer*>(get_node("BoltCooldown"))->stop();
}

void Player::_on_ExplosiveBoltCooldown_timeout()
{
	static_cast<Timer*>(get_node("ExplosiveBoltCooldown"))->stop();
}

void Player::_on_EntanglingBallsCooldown_timeout()
{
	static_cast<Timer*>(get_node("EntanglingBallsCooldown"))->stop();
}

void Player::init(int64_t chosenTeam, int64_t chosenRole)
{
	rpc("setTeam", chosenTeam);
	rpc("setRole", chosenRole);
	setUI(chosenRole);
	if (Team::CELADON == static_cast<Team>(chosenTeam))
		rpc("setSpawnPoint", get_node("/root/Game/World")->call("getCeladonTeamSpawnPoint"));
	else
		rpc("setSpawnPoint", get_node("/root/Game/World")->call("getCrimsonTeamSpawnPoint"));
	set_position(spawnPoint_);
}

void Player::setTeam(int64_t team)
{
	if (Team::CELADON == static_cast<Team>(team))
	{
		add_to_group("Celadon");
		animationNameSuffix_ = "_celadon";
	}
	else
	{
		add_to_group("Crimson");
		animationNameSuffix_ = "_crimson";
	}
}

void Player::setRole(int64_t role)
{
	role_ = role;
	if (Role::WARRIOR == static_cast<Role>(role_))
	{
		weapons_.push_back(static_cast<Axe*>(static_cast<Ref<PackedScene>>(resourceLoader_->load("res://equipment/weapons/Axe.tscn"))->instance()));
		get_node("melee_weapon_node")->add_child(weapons_[0]);
		currentWeapon_ = static_cast<Axe*>(weapons_[0]);
	}
	else
	{
		weapons_.push_back(static_cast<Dagger*>(static_cast<Ref<PackedScene>>(resourceLoader_->load("res://equipment/weapons/Dagger.tscn"))->instance()));
		weapons_.push_back(static_cast<Crossbow*>(static_cast<Ref<PackedScene>>(resourceLoader_->load("res://equipment/weapons/Crossbow.tscn"))->instance()));
		get_node("melee_weapon_node")->add_child(weapons_[0]);
		get_node("ranged_weapon_node")->add_child(weapons_[1]);
		static_cast<Node2D*>(get_node("melee_weapon_node"))->set_visible(false);
		currentWeapon_ = static_cast<Crossbow*>(weapons_[1]);
		setProjectileTypeTo(static_cast<int64_t>(ProjectileType::BOLT));
	}
	initialized_ = true;
}

void Player::setUI(int64_t role)
{
	if (Role::WARRIOR == static_cast<Role>(role_))
	{
		static_cast<TextureRect*>(ui_->get_node("Slot1/Icon"))->set_texture(resourceLoader_->load("res://sprites/icons/axe_icon.png"));
		static_cast<TextureRect*>(ui_->get_node("Slot2/Icon"))->set_texture(resourceLoader_->load("res://sprites/icons/entangling_balls_icon.png"));
		static_cast<TextureRect*>(ui_->get_node("Slot1/Highlight"))->set_visible(true);
	}
	else
	{
		static_cast<TextureRect*>(ui_->get_node("Slot1/Icon"))->set_texture(resourceLoader_->load("res://sprites/icons/dagger_icon.png"));
		static_cast<TextureRect*>(ui_->get_node("Slot2/Icon"))->set_texture(resourceLoader_->load("res://sprites/icons/crossbow_icon.png"));
		static_cast<CanvasItem*>(ui_->get_node("Slot2/Subslot1"))->set_visible(true);
		static_cast<CanvasItem*>(ui_->get_node("Slot2/Subslot2"))->set_visible(true);
		static_cast<TextureRect*>(ui_->get_node("Slot2/Highlight"))->set_visible(true);
		static_cast<TextureRect*>(ui_->get_node("Slot2/Subslot1/Highlight"))->set_visible(true);
	}
}

void Player::updateHealthPoints(int64_t newHealthPoints)
{
	if (healthPoints_ != newHealthPoints)
	{
		healthPoints_ = newHealthPoints;
		std::cout << "Player(" << getNodeName() << ") took some damage! His new health is: " << healthPoints_ << std::endl;
	}
}

void Player::updateHealthBar()
{
	healthBar_->setValue(healthPoints_);
}

void Player::updateMovementSpeed(int64_t newMovementSpeed)
{
	currentMovementSpeed_ = newMovementSpeed; 
}

void Player::setSlowTime(int64_t slowTime)
{
	static_cast<Timer*>(get_node("SlowTimer"))->set_wait_time(slowTime);
	static_cast<Timer*>(get_node("SlowTimer"))->start();
}

void Player::updateArmRotation(Vector2 aimingDirection)
{
	real_t angle = facingDirection_.angle_to(aimingDirection);
	static_cast<AnimatedSprite*>(get_node("right_hand_sprite"))->set_rotation(angle);
	static_cast<Crossbow*>(get_node("ranged_weapon_node"))->set_rotation(angle);
}

void Player::switchWeapon()
{
	if (currentWeapon_->getWeaponType() == static_cast<int64_t>(WeaponType::CROSSBOW))
	{
		currentWeapon_ = weapons_[0];
		static_cast<Node2D*>(get_node("melee_weapon_node"))->set_visible(true);
		static_cast<Node2D*>(get_node("ranged_weapon_node"))->set_visible(false);
		static_cast<AnimatedSprite*>(get_node("right_hand_sprite"))->set_rotation(0);
	}
	else if (currentWeapon_->getWeaponType() == static_cast<int64_t>(WeaponType::DAGGER))
	{
		currentWeapon_ = weapons_[1];
		static_cast<Node2D*>(get_node("melee_weapon_node"))->set_visible(false);
		static_cast<Node2D*>(get_node("ranged_weapon_node"))->set_visible(true);
	}
	currentWeapon_->playDrawSound();
}

void Player::setProjectileTypeTo(int64_t newProjectileType)
{
	currentAmmoType_ = static_cast<ProjectileType>(newProjectileType);
}

void Player::updateAimingDirection()
{
	Vector2 mousePosition = get_viewport()->get_mouse_position();
	Vector2 aimingDirection = Vector2(mousePosition.x - 960, mousePosition.y - 540);
	if (facingDirection_.x == 1 && aimingDirection.x < 0)
		aimingDirection.x = 0;
	else if (facingDirection_.x == -1 && aimingDirection.x > 0)
		aimingDirection.x = 0;

	aimingDirection_ = aimingDirection.normalized();
}