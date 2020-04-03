#include "Player.h"
#include <SceneTree.hpp>
#include <KinematicCollision2D.hpp>
#include <CollisionShape2D.hpp>
#include <TextureProgress.hpp>
#include <Label.hpp>
#include <Texture.hpp>
#include <Timer.hpp>
#include <Array.hpp>
#include <Input.hpp>
#include <SceneTree.hpp>
#include <AnimatedSprite.hpp>
#include <Sprite.hpp>
#include <AnimationPlayer.hpp>
#include <array>

using namespace godot;

void Player::_register_methods()
{
    register_method("_physics_process", &Player::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Player::_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Player::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Player::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_on_RespawnTimer_timeout", &Player::_on_RespawnTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_move", &Player::_move, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_HealthBar_value_changed", &Player::_on_HealthBar_value_changed, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("damage", &Player::damage, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("updateInput", &Player::updateInput, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("updateSprite", &Player::updateSprite, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_die", &Player::_die, GODOT_METHOD_RPC_MODE_SYNC);
    register_method("init", &Player::init, GODOT_METHOD_RPC_MODE_DISABLED);

    register_property<Player, Vector2>("slavePosition", &Player::slavePosition, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Player, int64_t>("slaveMovement", &Player::slaveMovement, static_cast<int64_t>(MoveDirection::NONE), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Player, int64_t>("slaveWeaponState", &Player::slaveWeaponState, static_cast<int64_t>(WeaponState::IDLE), GODOT_METHOD_RPC_MODE_PUPPET);

    /*
    GODOT_METHOD_RPC_MODE_DISABLED,
    GODOT_METHOD_RPC_MODE_REMOTE,
    GODOT_METHOD_RPC_MODE_MASTER,
    GODOT_METHOD_RPC_MODE_PUPPET,
    GODOT_METHOD_RPC_MODE_SLAVE = GODOT_METHOD_RPC_MODE_PUPPET,
    GODOT_METHOD_RPC_MODE_REMOTESYNC,
    GODOT_METHOD_RPC_MODE_SYNC = GODOT_METHOD_RPC_MODE_REMOTESYNC,
    GODOT_METHOD_RPC_MODE_MASTERSYNC,
    GODOT_METHOD_RPC_MODE_PUPPETSYNC,
    */
    register_property<Player, int64_t>("nodeName", &Player::nodeName, 0, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Player::_init()
{
	resourceLoader_ = ResourceLoader::get_singleton();
	velocity_ = Vector2(0, 0);
	moveDirection_ = MoveDirection::NONE;
	movementState_ = MovementState::NONE;
    slavePosition = Vector2();
    slaveMovement = static_cast<int64_t>(MoveDirection::NONE);
    nodeName = 0;

	Godot::print("[PLAYER] Player initialized.");
}

void Player::_ready()
{
	currentWeapon_ = static_cast<godot::Weapon*>(get_node("weapon_node/Weapon"));
	setWeapon(WeaponType::SWORD);
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
	if (movementState_ == MovementState::FALLING)
	{
		setVelocity(Vector2(getVelocity().x, getVelocity().y + GRAVITY_PULL));

		if (is_on_floor())
			movementState_ = MovementState::NONE;
	}

	switch (moveDirection)
	{
	case MoveDirection::NONE:
		velocity_.x = 0.0f;
		break;

	case MoveDirection::LEFT:
		velocity_.x = -MOVE_SPEED;

		break;

	case MoveDirection::RIGHT:
		velocity_.x = MOVE_SPEED;
		break;
	}

	move_and_slide(velocity_, Vector2(0, -1));
}

void Player::_on_HealthBar_value_changed(float value)
{
	
}

void Player::damage(int64_t value)
{
    healthPoints -= value;
    std::cout << "Damage, new health: " << healthPoints << std::endl;
    if (healthPoints <= 0)
    {
        healthPoints = 0;
        //rpc("_die");
    }
}

void Player::processAttack()
{
	if (weaponState_ == WeaponState::ATTACKING)
	{
		godot::Array overlapingBodies = currentWeapon_->get_overlapping_bodies();
		if (overlapingBodies.empty())
			return;
		
		currentWeapon_->set_physics_process(true);
		for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
		{
			godot::Player* attackedPlayer = static_cast<godot::Player*>(overlapingBodies[i]);
			if (attackedPlayer != this)
				attackedPlayer->damage(currentWeapon_->getDamage());
		}

		currentWeapon_->set_physics_process(false);
	}
	else
		return;
}

void Player::updateInput()
{
	moveDirection_ = MoveDirection::NONE;
	Input* input = Input::get_singleton();
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

    AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(get_node("weapon_animation"));
    if (input->is_action_just_pressed("basic_attack"))
    {
        if (!weaponAnimation->is_playing()) weaponState_ = WeaponState::ATTACKING;
    }
    else
    {
        if (!weaponAnimation->is_playing()) weaponState_ = WeaponState::IDLE;
    }
}

void Player::updateSprite()
{
    AnimatedSprite* bodySprite = static_cast<AnimatedSprite*>(get_node("body_sprite"));
    AnimatedSprite* leftHandSprite = static_cast<AnimatedSprite*>(get_node("left_hand_sprite"));
    AnimatedSprite* rightHandSprite = static_cast<AnimatedSprite*>(get_node("right_hand_sprite"));
    AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(get_node("weapon_animation"));
    godot::Weapon* weapon = static_cast<godot::Weapon*>(get_node("weapon_node"));

    if (moveDirection_ == MoveDirection::RIGHT)
    {
        bodySprite->play("walk");
        bodySprite->set_flip_h(false);
        rightHandSprite->set_flip_h(false);
        rightHandSprite->set_z_index(2);
        leftHandSprite->set_flip_h(false);
        leftHandSprite->set_z_index(1);
        weapon->set_z_index(-1);
        weapon->set_scale(Vector2(1, weapon->get_scale().y));
    }
    else if (moveDirection_ == MoveDirection::LEFT)
    {
        bodySprite->play("walk");
        bodySprite->set_flip_h(true);
        rightHandSprite->set_flip_h(true);
        rightHandSprite->set_z_index(-3);
        leftHandSprite->set_flip_h(true);
        leftHandSprite->set_z_index(3);
        weapon->set_z_index(-2);
        weapon->set_scale(Vector2(-1, weapon->get_scale().y));
    }
    else
    {
        bodySprite->play("idle");
    }

    if (!weaponAnimation->is_playing() && weaponState_ == WeaponState::ATTACKING)
    {
        weaponAnimation->play("attack");
        rightHandSprite->set_frame(0);
        rightHandSprite->play("attack");
    }
}

void Player::_die()
{
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

void Player::_on_RespawnTimer_timeout()
{
    set_physics_process(true);

    for (unsigned int i = 0; i < get_child_count(); i++)
    {
        if (get_child(i)->has_method("show"))
        {
            get_child(i)->call("show");
        }
    }
    static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(false);
    healthPoints = MAX_HP;
}

void Player::init(String nickname, Vector2 startPosition, bool isSlave)
{
    set_global_position(startPosition);
	Godot::print("Player " + nickname + " initialized.");
	//nickname_ = nickname; This line generates some unknown error and the program crashes
    //if (isSlave)
    //{

    //}
}

void Player::setWeapon(WeaponType weaponType)
{
	currentWeapon_->init(weaponType);
}
