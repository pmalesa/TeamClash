#include "Player.h"
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <KinematicCollision2D.hpp>
#include <CollisionShape2D.hpp>
#include <TextureProgress.hpp>
#include <Label.hpp>
#include <ResourceLoader.hpp>
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
	//nickname_ = "NewPlayer";

	//ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	//playerScene_ = resourceLoader->load("res://player/Player.tscn");


	//Label* nicknameLabel = static_cast<Label*>(get_node("/root/Player"));
	//nicknameLabel->set_text(nickname_);
	

	velocity_ = Vector2(0, 0);
	moveDirection_ = MoveDirection::NONE;
	movementState_ = MovementState::NONE;
    slavePosition = Vector2();
    slaveMovement = static_cast<int64_t>(MoveDirection::NONE);
    nodeName = 0;

    //ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    //weaponScene_ = resourceLoader->load("res://equipment/Weapon.tscn");


	Godot::print("Player initialized.");
}

void Player::_ready()
{
    //initialWeaponPosition_ = (static_cast<godot::Weapon*>(get_node("Weapon")))->get_position();
    //weapon_ = static_cast<godot::Weapon*>(weaponScene_->instance());
    //add_child(weapon_);
}

void Player::_physics_process(float delta)
{
	if (is_network_master())
	{
		rset_unreliable("slavePosition", get_position());
		rset("slaveMovement", static_cast<int64_t>(moveDirection_));
		_move(static_cast<int64_t>(moveDirection_));
	}
	else
	{
		_move(slaveMovement);
		set_position(slavePosition);
	}

	if (get_tree()->is_network_server())
	{
		//get_node("/root/Network")->call("update_position", get_name().to_int(), get_position());
	}

}

void Player::_process(float delta)
{
    if (is_network_master())
    {
        updateInput();
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
        rpc("_die");
    }
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
    std::array<AnimatedSprite*, 3> animatedSprites;
    AnimatedSprite* bodySprite = animatedSprites[0] = static_cast<AnimatedSprite*>(get_node("body_sprite"));
    AnimatedSprite* backHandSprite = animatedSprites[1] = static_cast<AnimatedSprite*>(get_node("back_hand_sprite"));
    AnimatedSprite* frontHandSprite = animatedSprites[2] = static_cast<AnimatedSprite*>(get_node("front_hand_sprite"));
    AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(get_node("weapon_animation"));
    //Sprite* weaponSprite = static_cast<Sprite*>(get_node("Weapon/weapon_sprite"));
    godot::Weapon* weapon = static_cast<godot::Weapon*>(get_node("weapon_node"));

    if (!is_network_master()) moveDirection_ = MoveDirection(slaveMovement);

    if (moveDirection_ == MoveDirection::LEFT)
    {
        bodySprite->play("walk");
        bodySprite->set_flip_h(true);
        if (weaponState_ != WeaponState::ATTACKING) backHandSprite->play("idle");
        backHandSprite->set_flip_h(true);
        frontHandSprite->play("walk");
        frontHandSprite->set_flip_h(true);
        weapon->set_z_index(-2);
        weapon->set_scale(Vector2(-1, weapon->get_scale().y));
        //weapon->set_position(Vector2(-initialWeaponPosition_.x + -bodySprite->get_position().x/2, weapon->get_position().y));
    }
    else if (moveDirection_ == MoveDirection::RIGHT)
    {
        bodySprite->play("walk");
        bodySprite->set_flip_h(false);
        backHandSprite->play("walk");
        backHandSprite->set_flip_h(false);
        if (weaponState_ != WeaponState::ATTACKING) frontHandSprite->play("idle");
        frontHandSprite->set_flip_h(false);
        weapon->set_z_index(-1);
        weapon->set_scale(Vector2(1, weapon->get_scale().y));
        //weapon->set_position(Vector2(initialWeaponPosition_.x, weapon->get_position().y));
    }
    else
    {
        bodySprite->play("idle");
        if (bodySprite->is_flipped_h())
        {
            if (weaponState_ != WeaponState::ATTACKING) backHandSprite->play("idle");
            frontHandSprite->play("idle");
        }
        else
        {
            if (weaponState_ != WeaponState::ATTACKING) frontHandSprite->play("idle");
            backHandSprite->play("idle");
        }
    }

    if (!weaponAnimation->is_playing() && weaponState_ == WeaponState::ATTACKING)
    {
        weaponAnimation->play("attack");
        if (bodySprite->is_flipped_h())
        {
            backHandSprite->set_frame(0);
            backHandSprite->play("attack");
        }
        else
        {
            frontHandSprite->set_frame(0);
            frontHandSprite->play("attack");
        }
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
	//nickname_ = nickname;
	//get_node("/root/Game")->call("setPlayerNickname", nickname_);

    //if (isSlave)
    //{

    //}
}
